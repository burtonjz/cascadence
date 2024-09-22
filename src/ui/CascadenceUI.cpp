#include <lv2/lv2plug.in/ns/ext/atom/atom.h>
#include <lv2/lv2plug.in/ns/ext/atom/forge.h>
#include <lv2/lv2plug.in/ns/lv2core/lv2_util.h>

#include "BStyles/Status.hpp"
#include "BWidgets/Supports/ValueableTyped.hpp"

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
    forgeBuf_(),
    uridMap_(nullptr),
    urids_(),
    widgets_(),
    iBypass_(100,100,200,100,
        bundlePath + "assets/iBypass.png")
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
    iBypass_.setClickable(true);
    iBypass_.setToggleable(true);
    iBypass_.setCallbackFunction(
        BEvents::Event::EventType::valueChangedEvent,
        [this] (BEvents::Event* ev) { valueChangedCallBack(ev) ; }
    );

    // add child widgets to parent widget and container
    add(&iBypass_) ;
    widgets_.push_back(&iBypass_) ;
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

void CascadenceUI::valueChangedCallBack(BEvents::Event* ev){
    BWidgets::Widget* widget = ev->getWidget() ;
    CascadenceUI* ui = dynamic_cast<CascadenceUI*>(widget->getMainWindow()) ;

    if (!ui) return ;

    if ( widget != &iBypass_ ) return ;

    bool value = dynamic_cast<BWidgets::ValueableTyped<bool>*>(widget)->getValue() ;

    if (value){
        iBypass_.image.loadImage(BStyles::Status::normal, bundlePath_ + "assets/iBypass_pressed.png");
    } else {
        iBypass_.image.loadImage(BStyles::Status::normal, bundlePath_ + "assets/iBypass_unpressed.png");
    }

    // write a set message to notify plugin of the change
    lv2_atom_forge_set_buffer(
        &ui->forge_,
        ui->forgeBuf_,
        sizeof(ui->forgeBuf_)
    );

    LV2_Atom_Forge_Frame frame ;
    LV2_Atom* msg = reinterpret_cast<LV2_Atom*>(lv2_atom_forge_object(
        &forge_, &frame, 0, urids_.patchSet
    )) ;

    lv2_atom_forge_key(&forge_,urids_.patchProperty);
    lv2_atom_forge_urid(&forge_, urids_.plugBypass);
    lv2_atom_forge_key(&forge_, urids_.patchValue);
    lv2_atom_forge_bool(&forge_, value);
    lv2_atom_forge_pop(&forge_, &frame);

    writeFunction_(
        ui->controller_,
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
