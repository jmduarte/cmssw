
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/stream/EDProducer.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/Framework/interface/makeRefToBaseProdFrom.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Utilities/interface/StreamID.h"

#include "DataFormats/BTauReco/interface/JetTag.h"

#include "DataFormats/DeepFormats/interface/DeepFlavourTagInfo.h"

#include "DNN/Tensorflow/interface/Graph.h"
#include "DNN/Tensorflow/interface/Tensor.h"

#include "RecoBTag/DeepFlavour/interface/tensor_fillers.h"



class DeepFlavourJetTagProducer : public edm::stream::EDProducer<> {

  public:
	  explicit DeepFlavourJetTagProducer(const edm::ParameterSet&);
	  ~DeepFlavourJetTagProducer();

	  static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);

  private:

    typedef std::vector<reco::DeepFlavourTagInfo> TagInfoCollection;
    typedef reco::JetTagCollection JetTagCollection;

	  virtual void beginStream(edm::StreamID) override {}
	  virtual void produce(edm::Event&, const edm::EventSetup&) override;
	  virtual void endStream() override {}

	  const edm::EDGetTokenT< TagInfoCollection > src_;
    std::string graph_path_;
    std::vector<std::pair<std::string,std::vector<unsigned int>>> flav_pairs_;
    std::vector<std::string> input_names_;
    std::vector<std::string> output_names_;

    // graph for TF evaluation
    dnn::tf::Graph graph_;
    // not owing vector of pointers for inputs and outputs
    std::vector<dnn::tf::Tensor *> dnn_inputs_;
    std::vector<dnn::tf::Tensor *> dnn_outputs_;


};

DeepFlavourJetTagProducer::DeepFlavourJetTagProducer(const edm::ParameterSet& iConfig) :
  src_(consumes<TagInfoCollection>(iConfig.getParameter<edm::InputTag>("src"))),
  graph_path_(iConfig.getParameter<std::string>("graph_path")),
  input_names_(iConfig.getParameter<std::vector<std::string>>("input_names")),
  output_names_(iConfig.getParameter<std::vector<std::string>>("output_names")),
  graph_(graph_path_)
{

  // get output names from flav_table
  const auto & flav_pset = iConfig.getParameter<edm::ParameterSet>("flav_table");
  for (const auto flav_pair : flav_pset.tbl()) {
    const auto & flav_name = flav_pair.first;
    flav_pairs_.emplace_back(flav_name,
                                flav_pset.getParameter<std::vector<unsigned int>>(flav_name));
  }


  for (const auto & flav_pair : flav_pairs_) {
    produces<JetTagCollection>(flav_pair.first);
  }

  for (const auto & input_name : input_names_) {
    dnn_inputs_.emplace_back(graph_.defineInput(new dnn::tf::Tensor(input_name)));
  }

  // required because of batch norm
  // names for the learing phase placeholders (to init and set as false)
  const auto & lp_names = iConfig.getParameter<std::vector<std::string>>("lp_names");
  for (const auto & lp_name : lp_names) {
    auto input_ptr = graph_.defineInput(new dnn::tf::Tensor(lp_name));
    input_ptr->setArray(0, nullptr);
    input_ptr->setValue<bool>(false);
  }

  for (const auto & output_name : output_names_) {
    dnn_outputs_.emplace_back(graph_.defineOutput(new dnn::tf::Tensor(output_name)));
  }


  

}


DeepFlavourJetTagProducer::~DeepFlavourJetTagProducer()
{
}

void DeepFlavourJetTagProducer::fillDescriptions(edm::ConfigurationDescriptions& descriptions)
{
}

void DeepFlavourJetTagProducer::produce(edm::Event& iEvent, const edm::EventSetup& iSetup)
{

  edm::Handle<TagInfoCollection> tag_infos;
  iEvent.getByToken(src_, tag_infos);

  std::vector<std::unique_ptr<JetTagCollection>> output_tags;

  auto n_jets = dnn::tf::Shape(tag_infos->size());
  std::vector<std::vector<dnn::tf::Shape>> input_sizes {
    {n_jets, 15},         // input_1 - global jet features
    {n_jets, 25, 16},     // input_2 - charged pf
    {n_jets, 25, 6},      // input_3 - neutral pf
    {n_jets, 4, 12},      // input_4 - vertices 
    {n_jets, 1}           // input_5 - jet pt for reg 
  };
  
  // initalize inputs
  // CMMSW-DNN sets zeros by default
  for (std::size_t i=0; i < input_sizes.size(); i++) {
    auto & input_shape = input_sizes.at(i);
    dnn_inputs_.at(i)->setArray(input_shape.size(),
                                &input_shape[0]);
  }

  // fill values
  for (std::size_t jet_n=0; jet_n < tag_infos->size(); jet_n++) {

    // jet and other global features
    const auto & features = tag_infos->at(jet_n).features();
    jet_tensor_filler(dnn_inputs_.at(0), jet_n, features);
    // c_pf candidates
    auto max_c_pf_n = std::min(features.c_pf_features.size(),
                               (std::size_t) input_sizes.at(1).at(1));
    for (std::size_t c_pf_n=0; c_pf_n < max_c_pf_n; c_pf_n++) {
      const auto & c_pf_features = features.c_pf_features.at(c_pf_n);
      c_pf_tensor_filler(dnn_inputs_.at(1), jet_n, c_pf_n, c_pf_features);
    }
    // n_pf candidates
    auto max_n_pf_n = std::min(features.n_pf_features.size(),
                               (std::size_t) input_sizes.at(2).at(1));
    for (std::size_t n_pf_n=0; n_pf_n < max_n_pf_n; n_pf_n++) {
      const auto & n_pf_features = features.n_pf_features.at(n_pf_n);
      n_pf_tensor_filler(dnn_inputs_.at(2), jet_n, n_pf_n, n_pf_features);
    }
    // sv candidates
    auto max_sv_n = std::min(features.sv_features.size(),
                               (std::size_t) input_sizes.at(3).at(1));
    for (std::size_t sv_n=0; sv_n < max_sv_n; sv_n++) {
      const auto & sv_features = features.sv_features.at(sv_n);
      sv_tensor_filler(dnn_inputs_.at(3), jet_n, sv_n, sv_features);
    }
    // last input: corrected jet pt
    dnn_inputs_.at(4)->setValue(jet_n, 0, features.jet_features.corr_pt);

  }

  // compute graph
  graph_.eval();


  // create output collection
  for (std::size_t i=0; i < flav_pairs_.size(); i++) {
    if (tag_infos->size() > 0) {
      auto jet_ref = tag_infos->begin()->jet();
      output_tags.emplace_back(std::make_unique<JetTagCollection>(
            edm::makeRefToBaseProdFrom(jet_ref, iEvent)));
    } else {
      output_tags.emplace_back(std::make_unique<JetTagCollection>());
    }
  }

  // set output values for flavour probs
  for (std::size_t jet_n=0; jet_n < tag_infos->size(); jet_n++) {
    const auto & jet_ref = tag_infos->at(jet_n).jet();
    for (std::size_t flav_n=0; flav_n < flav_pairs_.size(); flav_n++) {
      const auto & flav_pair = flav_pairs_.at(flav_n);
      float o_sum = 0.;
      for (const unsigned int & ind : flav_pair.second) {
        o_sum +=  dnn_outputs_.at(0)->getValue<float>((dnn::tf::Shape) jet_n,
                                                      (dnn::tf::Shape) flav_n,
                                                      (dnn::tf::Shape) ind);
      } 
      (*output_tags.at(flav_n))[jet_ref] = o_sum;
    }
  }

  for (std::size_t i=0; i < flav_pairs_.size(); i++) {
    iEvent.put(std::move(output_tags.at(i)), flav_pairs_.at(i).first);
  }

}

//define this as a plug-in
DEFINE_FWK_MODULE(DeepFlavourJetTagProducer);
