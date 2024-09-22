#include <lv2/lv2plug.in/ns/lv2core/lv2.h>
#include <lv2/lv2plug.in/ns/extensions/ui/ui.h>

#include <cstring>
#include <iostream>
#include <cstdint>
#include <string>

#include "urids.hpp"
#include "CascadenceUI.hpp"

#include <iostream>

static LV2UI_Handle instantiate(
    const struct LV2UI_Descriptor *descriptor,
    const char *plugin_uri,
    const char *bundle_path,
    LV2UI_Write_Function write_function,
    LV2UI_Controller controller,
    LV2UI_Widget *widget,
    const LV2_Feature *const *features)
{

    std::cout << "[Cascadence] Instantiating UI" << std::endl ;

    // make sure we're called by the right plugin
    if (std::strcmp(plugin_uri,CASCADENCE_URI) != 0) return nullptr ;

    // define window
    void* parentXWindow = nullptr;
    for (int i = 0; features[i]; ++i){
        if (strcmp (features[i]->URI, LV2_UI__parent) == 0){
            parentXWindow = features[i]->data ;
            break ;
        }
    }

    if (!parentXWindow){
        std::cerr << "Required feature LV2_UI__parent not provided" << std::endl ;
        return nullptr ;
    }

    CascadenceUI* ui ;
    try {
        ui = new CascadenceUI(
            write_function,
            controller,
            parentXWindow,
            std::string(bundle_path),
            features
        ) ;
    } catch(std::exception& exc) {
        std::cerr << "[Cascadence] UI instantiation failed." << std::endl ;
        return nullptr ;
    }

    std::cout << "[Cascadence] UI Instantiated" << std::endl ;

    *widget = ui->getTopLevelWidget() ;
    return static_cast<LV2UI_Handle>(ui) ;
}

static void cleanup(LV2UI_Handle ui){
    CascadenceUI* cui = static_cast<CascadenceUI*>(ui) ;
    if (cui) delete cui ;
}

static void port_event(
    LV2UI_Handle ui,
    uint32_t port_index,
    uint32_t buffer_size,
    uint32_t format,
    const void *buffer)
{
    CascadenceUI* cui = static_cast<CascadenceUI*> (ui);
    if(cui) cui->portEvent(port_index,buffer_size,format,buffer);
}

static int ui_idle(LV2UI_Handle ui){
    CascadenceUI* cui = static_cast<CascadenceUI*> (ui);
    cui->handleEvents();
    return 0;
}

static const void *extension_data(const char *uri){
    static const LV2UI_Idle_Interface idle = { ui_idle };
    if (strcmp (uri, LV2_UI__idleInterface) == 0) return &idle;
    return nullptr;
}

static const LV2UI_Descriptor ui_descriptor =
{
    CASCADENCE_URI "#ui",
    instantiate,
    cleanup,
    port_event,
    extension_data
};

LV2_SYMBOL_EXPORT const LV2UI_Descriptor* lv2ui_descriptor(uint32_t index){
    switch(index){
        case 0: return &ui_descriptor;
        default: return 0;
    }
}
