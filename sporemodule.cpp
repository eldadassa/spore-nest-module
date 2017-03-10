#include "sporemodule.h"
#include "spore.h"

#if defined(__SPORE_WITH_NEST_2_10__)

// include necessary NEST headers
#include "config.h"
#include "network.h"
#include "model.h"
#include "dynamicloader.h"
#include "exceptions.h"
#include "sliexceptions.h"
#include "nestmodule.h"
#include "target_identifier.h"

#elif defined(__SPORE_WITH_NEST_2_12__)

// Includes from nestkernel:
#include "connection_manager_impl.h"
#include "connector_model_impl.h"
#include "dynamicloader.h"
#include "exceptions.h"
#include "genericmodel.h"
#include "genericmodel_impl.h"
#include "kernel_manager.h"
#include "model.h"
#include "model_manager_impl.h"
#include "nestmodule.h"
#include "target_identifier.h"

#else
#error NEST version is not supported!
#endif


// Include the module's headers
#include "connection_updater.h"
#include "diligent_connector_model.h"

#include "poisson_dbl_exp_neuron.h"
#include "synaptic_sampling_rewardgradient_connection.h"
#include "reward_in_proxy.h"

#ifdef __SPORE_DEBUG__
#include "spore_test_node.h"
#include "spore_test_connection.h"
#endif

/*
 * The dynamic module loader must be able to find your module.
 * You make the module known to the loader by defining an instance of your
 * module class in global scope. This instance must have the name
 *
 * <modulename>_LTX_mod
 *
 * The dynamicloader can then load modulename and search for symbol "mod" in it.
 */
spore::SporeModule sporemodule_LTX_mod;


/**
 * Constructor.
 */
spore::SporeModule::SporeModule()
{
#ifdef LINKED_MODULE
    // register this module at the dynamic loader
    // this is needed to allow for linking in this module at compile time
    // all registered modules will be initialized by the main app's dynamic loader
    nest::DynamicLoaderModule::registerLinkedModule(this);
#endif
}


/**
 * Destructor.
 */
spore::SporeModule::~SporeModule()
{
}


/**
 * Return the name of the model.
 */
const std::string spore::SporeModule::name(void) const
{
    return std::string("SPORE (version 0.5)"); // Return name of the module
}


/**
 * Return the name of a sli file to execute when sporemodule is loaded.
 * This mechanism can be used to define SLI commands associated with your
 * module, in particular, set up type tries for functions you have defined.
 */
const std::string spore::SporeModule::commandstring(void) const
{
    // Instruct the interpreter to load sporemodule-init.sli
    return std::string("(sporemodule-init) run");
}


/**
 * Constructor.
 */
spore::SporeModule::
InitSynapseUpdater_i_i_Function::InitSynapseUpdater_i_i_Function()
{
}


/**
 * Initializes the synapse updater nodes.
 *
 * @param i   pointer to the SLI interpreter.
 */
void spore::SporeModule::
InitSynapseUpdater_i_i_Function::execute(SLIInterpreter *i) const
{
    // Check if we have (at least) five arguments on the stack.
    i->assert_stack_load(2);

    const long interval = getValue<long>(i->OStack.pick(1)); // bottom
    const long delay = getValue<long>(i->OStack.pick(0)); // top

    ConnectionUpdateManager::instance()->setup(interval, delay);

    i->OStack.pop(2);
    i->EStack.pop();
}


/**
 * Initialize module by registering models with the interpreter.
 * @param SLIInterpreter* SLI interpreter
 * @note  Parameter Network is needed for historical compatibility
 *        only.
 */
void spore::SporeModule::init(SLIInterpreter *i)
{
    
#if defined(__SPORE_WITH_NEST_2_10__)

    nest::Network& network = nest::NestModule::get_network();

    ConnectionUpdateManager::instance()->init();

    // Register nodes
    nest::register_model<PoissonDblExpNeuron>(network, "poisson_dbl_exp_neuron");
    nest::register_model<RewardInProxy>(network, "reward_in_proxy");

    spore::register_diligent_connection_model< SynapticSamplingRewardGradientConnection<nest::TargetIdentifierPtrRport> >("synaptic_sampling_rewardgradient_synapse");

    i->createcommand("InitSynapseUpdater", &init_synapse_updater_i_i_function_);

#ifdef __SPORE_DEBUG__
    nest::register_model<SporeTestNode>(network, "spore_test_node");
    spore::register_diligent_connection_model<SporeTestConnection<nest::TargetIdentifierPtrRport> >("spore_test_synapse");
#endif

#elif defined(__SPORE_WITH_NEST_2_12__)

    ConnectionUpdateManager::instance()->init();

    // Register nodes
    nest::kernel().model_manager.register_node_model<PoissonDblExpNeuron>("poisson_dbl_exp_neuron");
    nest::kernel().model_manager.register_node_model<RewardInProxy>("reward_in_proxy");

    spore::register_diligent_connection_model< SynapticSamplingRewardGradientConnection<nest::TargetIdentifierPtrRport> >("synaptic_sampling_rewardgradient_synapse");

    i->createcommand("InitSynapseUpdater", &init_synapse_updater_i_i_function_);

#ifdef __SPORE_DEBUG__
    nest::kernel().model_manager.register_node_model<SporeTestNode>("spore_test_node");
    spore::register_diligent_connection_model<SporeTestConnection<nest::TargetIdentifierPtrRport> >("spore_test_synapse");
#endif

#else
#error NEST version is not supported!
#endif

}