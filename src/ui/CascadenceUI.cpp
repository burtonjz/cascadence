#include <lv2/lv2plug.in/ns/ext/atom/atom.h>
#include <lv2/lv2plug.in/ns/ext/atom/forge.h>
#include <lv2/lv2plug.in/ns/lv2core/lv2_util.h>

#include <type_traits>

#include "BEvents/Event.hpp"
#include "BStyles/Status.hpp"
#include "BWidgets/Supports/ValueableTyped.hpp"

#include "config.hpp"
#include "CascadenceUI.hpp"
#include "pugl/pugl.h"

CascadenceUI::CascadenceUI(
    LV2UI_Write_Function writeFunction,
    LV2UI_Controller controller,
    void* parentXWindow,
    std::string bundlePath,
    const LV2_Feature *const *features
):
    Window(
        BWIDGETS_DEFAULT_WINDOW_WIDTH,
        BWIDGETS_DEFAULT_WINDOW_HEIGHT,
        reinterpret_cast<PuglNativeView>(parentXWindow),
        0,
        "Cascadence",
        false,
        PUGL_MODULE,
        0
    ),
    writeFunction_(writeFunction),
    controller_(controller),
    bundlePath_(bundlePath),
    features_(features),
    forge_(),
    uridMap_(nullptr),
    urids_(),
    widgets_(),
    wBypass_(100,100,200,100,
        bundlePath + "assets/wBypass.png"),
    wBpm_(310,100,50,400, CONFIG_DEFAULT_BPM, 20, 300, 5)
{

    std::cout << "[Cascadence] This delay seems to stop a segmentation fault..." << std::endl ;

    // check features
    const char* missing = lv2_features_query(
        features_,
        LV2_URID__map, &uridMap_, true
    );

    if (missing) throw ;

    urids_.initialize(uridMap_);
    lv2_atom_forge_init(&forge_, uridMap_);

    // std::cout << "[Cascadence] 5" << std::endl ;

    // set child widget properties
    wBypass_.setClickable(true);
    wBypass_.setToggleable(true);
    wBypass_.setCallbackFunction(
        BEvents::Event::EventType::valueChangedEvent,
        [this] (BEvents::Event* ev) { bypassCallback(ev) ; }
    );

    wBpm_.setClickable(true);
    wBpm_.setDraggable(true);
    wBpm_.setCallbackFunction(
        BEvents::Event::EventType::valueChangedEvent,
        [this] (BEvents::Event* ev) { bpmCallback(ev) ; }
    );

    // add child widgets to parent widget and container
    add(&wBypass_) ;
    widgets_.push_back(&wBypass_) ;

    add(&wBpm_);
    widgets_.push_back(&wBpm_);
}

void CascadenceUI::portEvent(
    uint32_t port_index,
    uint32_t buffer_size,
    uint32_t format,
    const void *buffer
){
    if ( format == urids_.atomEventTransfer ){
        const LV2_Atom *atom = reinterpret_cast<const LV2_Atom*>(buffer) ;
        if (lv2_atom_forge_is_object_type(&forge_, atom->type)){
            const LV2_Atom_Object* obj = reinterpret_cast<const LV2_Atom_Object*>(atom);

            if      (obj->body.otype == urids_.patchGet) handlePatchGet(obj) ;
            else if (obj->body.otype == urids_.patchSet) handlePatchSet(obj) ;
            else if (obj->body.otype == urids_.patchPut) handlePatchPut(obj) ;

        }
    }
}

LV2UI_Widget CascadenceUI::getTopLevelWidget(){
    return reinterpret_cast<LV2UI_Widget>(puglGetNativeView(getView()));
}

void CascadenceUI::bypassCallback(BEvents::Event* ev){
    bool value = wBypass_.getValue() ;

    if (value){
        std::cout << "loading bypass button image" << std::endl ;
        wBypass_.image.loadImage(BStyles::Status::normal, bundlePath_ + "assets/wBypass_pressed.png");
    } else {
        std::cout << "loading bypass button image" << std::endl ;
        wBypass_.image.loadImage(BStyles::Status::normal, bundlePath_ + "assets/wBypass_unpressed.png");
    }

    sendValueChangedAtom<bool>(urids_.plugBypass, value);
}

void CascadenceUI::bpmCallback(BEvents::Event* ev){
    int value = wBpm_.getValue();
    sendValueChangedAtom<int>(urids_.plugBpm, value);
}

template<typename T>
void CascadenceUI::sendValueChangedAtom(LV2_URID urid, T value){
    uint8_t buffer[1024] ;

    // write a set message to notify plugin of the change
    lv2_atom_forge_set_buffer(
        &forge_,
        buffer,
        sizeof(buffer)
    );

    LV2_Atom_Forge_Frame frame ;
    LV2_Atom* msg = reinterpret_cast<LV2_Atom*>(lv2_atom_forge_object(
        &forge_, &frame, 0, urids_.patchSet
    )) ;

    lv2_atom_forge_key(&forge_,urids_.patchProperty);
    lv2_atom_forge_urid(&forge_, urid);
    lv2_atom_forge_key(&forge_, urids_.patchValue);
    if        ( std::is_same<T,bool>::value ){
        lv2_atom_forge_bool(&forge_, value);
    } else if ( std::is_same<T,int>::value ){
        lv2_atom_forge_int(&forge_,value);
    } else if ( std::is_same<T,float>::value ){
        lv2_atom_forge_float(&forge_,value);
    }
    lv2_atom_forge_pop(&forge_, &frame);

    writeFunction_(
        controller_,
        0,
        lv2_atom_total_size(msg),
        urids_.atomEventTransfer,
        msg
    );
}

void CascadenceUI::handlePatchGet(const LV2_Atom_Object* obj){

}

void CascadenceUI::handlePatchSet(const LV2_Atom_Object* obj){

}

void CascadenceUI::handlePatchPut(const LV2_Atom_Object* obj){

}