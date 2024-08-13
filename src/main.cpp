#include <lv2.h>
#include <lv2/lv2plug.in/ns/lv2core/lv2.h>
#include <lv2/lv2plug.in/ns/ext/state/state.h>

#include <iostream>

#include "Cascadence.hpp"
#include "ParameterController.hpp"

static LV2_Handle instantiate(const struct LV2_Descriptor *descriptor, double sample_rate, const char *bundle_path, const LV2_Feature *const *features)
{
    std::cout << "[" << CASCADENCE_URI << "]: Instantiating plugin." << std::endl ;

    Cascadence* m = nullptr ;
    try {
        m = new Cascadence(sample_rate, features);
    } 
    catch(const std::invalid_argument& ia){
        std::cerr << ia.what() << std::endl ;
        return nullptr ;
    }
    catch(const std::bad_alloc& ba){
        std::cerr << "Failed to allocate memory. Aborting." << std::endl ;
        return nullptr ;
    }
    
    std::cout << "[" << CASCADENCE_URI << "]: Plugin Instantiated." << std::endl ;
    return m ;
}

static void connect_port(LV2_Handle instance, uint32_t port, void *data){
    Cascadence* m = static_cast <Cascadence*> (instance);
    if (m) m->connectPort (port, data);
}

static void activate(LV2_Handle instance){
    Cascadence* m = static_cast <Cascadence*> (instance);
    m->activate();
}

static void run(LV2_Handle instance, uint32_t sample_count){
    Cascadence* m = static_cast <Cascadence*> (instance);

    if (m) m->run (sample_count);
}

static void deactivate (LV2_Handle instance){
    Cascadence* m = static_cast <Cascadence*> (instance);
    m->deactivate();
}

static void cleanup (LV2_Handle instance){
    Cascadence* m = static_cast <Cascadence*> (instance);
    if (m) delete m ;
}

static const void* extension_data(const char *uri){
    return NULL ;
}


static LV2_Descriptor const descriptor = {
    CASCADENCE_URI,
    instantiate,
    connect_port,
    activate,
    run,
    deactivate,
    cleanup,
    extension_data
};
LV2_SYMBOL_EXPORT const LV2_Descriptor* lv2_descriptor(uint32_t index){
    if (index == 0) return &descriptor ;
    return NULL ;
};

// STATE INTERFACE FUNCTIONS
static LV2_State_Status save(
    LV2_Handle                instance,
    LV2_State_Store_Function  store,
    LV2_State_Handle          handle,
    uint32_t                  flags,
    const LV2_Feature* const* features
){
    Cascadence* m = static_cast <Cascadence*> (instance) ;
    ParameterController* p = m->getParameterController() ;
    return p->saveState(store, handle, flags, features);
};

static LV2_State_Status restore(
    LV2_Handle                  instance,
    LV2_State_Retrieve_Function retrieve,
    LV2_State_Handle            handle,
    uint32_t                    flags,
    const LV2_Feature* const*   features
){
    Cascadence* m = static_cast <Cascadence*> (instance) ;
    ParameterController* p = m->getParameterController() ;
    return p->restoreState(retrieve, handle, flags, features);
};


