#ifndef __CASCADENCE_UI_HPP_
#define __CASCADENCE_UI_HPP_

#include <lv2/lv2plug.in/ns/lv2core/lv2.h>
#include <lv2/lv2plug.in/ns/extensions/ui/ui.h>
#include <lv2/lv2plug.in/ns/ext/atom/forge.h>
#include <lv2/urid/urid.h>

#include <string>
#include <vector>

#include "BWidgets/Window.hpp"
#include "BWidgets/CheckBox.hpp"
#include "BEvents/Event.hpp"

#include "urids.hpp"



class CascadenceUI : public BWidgets::Window {
private:
    LV2UI_Write_Function writeFunction_ ;
    LV2UI_Controller controller_ ;
    std::string bundlePath_ ;
    const LV2_Feature *const *features_ ;

    LV2_Atom_Forge forge_ ;
    uint8_t forgeBuf_[1024] ;
    LV2_URID_Map* uridMap_ ;
    Urids urids_ ;

    // widgets
    std::vector<BWidgets::Widget*> widgets_ ;
    BWidgets::CheckBox checkBoxBypass_ ;

public:
    CascadenceUI(
        LV2UI_Write_Function writeFunction,
        LV2UI_Controller controller,
        void* parentXWindow,
        std::string bundlePath,
        const LV2_Feature *const *features
    );

    void portEvent(
        uint32_t port_index,
        uint32_t buffer_size,
        uint32_t format,
        const void *buffer
    );

    LV2UI_Widget getTopLevelWidget();

    void valueChangedCallBack(BEvents::Event* ev);

private:
    void handlePatchGet(const LV2_Atom_Object* obj);
    void handlePatchSet(const LV2_Atom_Object* obj);
    void handlePatchPut(const LV2_Atom_Object* obj);

};

#endif // __CASCADENCE_UI_HPP_
