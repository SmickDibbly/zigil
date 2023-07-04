#ifndef ZGLX_MACRO_STR_H
#define ZGLX_MACRO_STR_H

/* Intended just for zglx.c hence the static storage class in header. */

#include <X11/X.h>

#define stringify(M) [M] = #M
static char const *const Mapping_strs[] =
    {
     stringify(MappingModifier),
     stringify(MappingKeyboard),
     stringify(MappingPointer),
    };
static char const *const Button_strs[] =
    {
     stringify(Button1),
     stringify(Button2),
     stringify(Button3),
     stringify(Button4),
     stringify(Button5),
    };
static char const *const Modifier_strs[] =
    {
     stringify(ShiftMapIndex),
     stringify(LockMapIndex),
     stringify(ControlMapIndex),
     stringify(Mod1MapIndex),
     stringify(Mod2MapIndex),
     stringify(Mod3MapIndex),
     stringify(Mod4MapIndex),
     stringify(Mod5MapIndex),
    };
static char const *const NotifyMode_strs[] =
    {
     stringify(NotifyNormal),
     stringify(NotifyGrab),
     stringify(NotifyUngrab),
     stringify(NotifyWhileGrabbed),
     //potentially missing NotifyHint which is also index 1 like NotifyGrab
    };
static char const *const NotifyDetail_strs[] =
    {
     stringify(NotifyAncestor),
     stringify(NotifyVirtual),
     stringify(NotifyInferior),
     stringify(NotifyNonlinear),
     stringify(NotifyNonlinearVirtual),
     stringify(NotifyPointer),
     stringify(NotifyPointerRoot),
     stringify(NotifyDetailNone),
    };
static char const *const Visibility_strs[] =
    {
     stringify(VisibilityUnobscured),
     stringify(VisibilityPartiallyObscured),
     stringify(VisibilityFullyObscured),
    };
static char const *const Circulation_strs[] =
    {
     stringify(PlaceOnTop),
     stringify(PlaceOnBottom),
    };
static char const *const ProtocolFamily_strs[] =
    {
     stringify(FamilyInternet),
     stringify(FamilyDECnet),
     stringify(FamilyChaos),
     stringify(FamilyInternet6),
    };
static char const *const Property_strs[] =
    {
     stringify(PropertyNewValue),
     stringify(PropertyDelete),
    };
static char const *const ColorMap_strs[] =
    {
     stringify(ColormapUninstalled),
     stringify(ColormapInstalled),
    };
static char const *const GrabMode_strs[] =
    {
     stringify(GrabModeSync),
     stringify(GrabModeAsync),
    };
static char const *const GrabStatus_strs[] =
    {
     stringify(GrabSuccess),
     stringify(AlreadyGrabbed),
     stringify(GrabInvalidTime),
     stringify(GrabNotViewable),
     stringify(GrabFrozen),
    };
static char const *const AllowEvents_strs[] =
    {
     stringify(AsyncPointer),
     stringify(SyncPointer),
     stringify(ReplayPointer),
     stringify(AsyncKeyboard),
     stringify(SyncKeyboard),
     stringify(ReplayKeyboard),
     stringify(AsyncBoth),
     stringify(SyncBoth),
    };
static char const *const Class_strs[] =
    {
     stringify(InputOutput),
     stringify(InputOnly)
    };
static char const *const Bit_Gravity_strs[] =
    {
     stringify(ForgetGravity),
     stringify(NorthWestGravity),
     stringify(NorthGravity),
     stringify(NorthEastGravity),
     stringify(WestGravity),
     stringify(CenterGravity),
     stringify(EastGravity),
     stringify(SouthWestGravity),
     stringify(SouthGravity),
     stringify(SouthEastGravity),
     stringify(StaticGravity),
    };
static char const *const Window_Gravity_strs[] =
    {
     stringify(UnmapGravity),
     stringify(NorthWestGravity),
     stringify(NorthGravity),
     stringify(NorthEastGravity),
     stringify(WestGravity),
     stringify(CenterGravity),
     stringify(EastGravity),
     stringify(SouthWestGravity),
     stringify(SouthGravity),
     stringify(SouthEastGravity),
     stringify(StaticGravity),
    };
static char const *const Backing_Store_strs[] =
    {
     stringify(NotUseful),
     stringify(WhenMapped),
     stringify(Always),
    };
static char const *const Map_State_strs[] =
    {
     stringify(IsUnmapped),
     stringify(IsUnviewable),
     stringify(IsViewable),
    };
static char const *const Line_Style_strs[] =
    {
     stringify(LineSolid),
     stringify(LineOnOffDash),
     stringify(LineDoubleDash),
    };
static char const *const Cap_Style_strs[] =
    {
     stringify(CapNotLast),
     stringify(CapButt),
     stringify(CapRound),
     stringify(CapProjecting),
    };
static char const *const Join_Style_strs[] =
    {
     stringify(JoinMiter),
     stringify(JoinRound),
     stringify(JoinBevel),
    };
static char const *const Fill_Style_strs[] =
    {
     stringify(FillSolid),
     stringify(FillTiled),
     stringify(FillStippled),
     stringify(FillOpaqueStippled),
    };
static char const *const Fill_Rule_strs[] =
    {
     stringify(EvenOddRule),
     stringify(WindingRule),
    };
static char const *const Subwindow_Mode_strs[] =
    {
     stringify(ClipByChildren),
     stringify(IncludeInferiors),
    };
static char const *const Arc_Mode_strs[] =
    {
     stringify(ArcChord),
     stringify(ArcPieSlice),
    };
static char const *const Byte_Order_strs[] =
    {
     stringify(LSBFirst),
     stringify(MSBFirst),
    };
static char const *const Display_Class_strs[] =
    {
     stringify(StaticGray),
     stringify(GrayScale),
     stringify(StaticColor),
     stringify(PseudoColor),
     stringify(TrueColor),
     stringify(DirectColor),
    };
static char const *const Image_Format_strs[] =
    {
     stringify(XYBitmap),
     stringify(XYPixmap),
     stringify(ZPixmap),
    };
static char const *const Event_Type_strs[] =
    {
     stringify(KeyPress),
     stringify(KeyRelease),
     stringify(ButtonPress),
     stringify(ButtonRelease),
     stringify(MotionNotify),
     stringify(EnterNotify),
     stringify(LeaveNotify),
     stringify(FocusIn),
     stringify(FocusOut),
     stringify(KeymapNotify),
     stringify(Expose),
     stringify(GraphicsExpose),
     stringify(NoExpose),
     stringify(VisibilityNotify),
     stringify(CreateNotify),
     stringify(DestroyNotify),
     stringify(UnmapNotify),
     stringify(MapNotify),
     stringify(MapRequest),
     stringify(ReparentNotify),
     stringify(ConfigureNotify),
     stringify(ConfigureRequest),
     stringify(GravityNotify),
     stringify(ResizeRequest),
     stringify(CirculateNotify),
     stringify(CirculateRequest),
     stringify(PropertyNotify),
     stringify(SelectionClear),
     stringify(SelectionRequest),
     stringify(SelectionNotify),
     stringify(ColormapNotify),
     stringify(ClientMessage),
     stringify(MappingNotify),
     stringify(GenericEvent),
     stringify(LASTEvent),
    };
#undef stringify

#endif /* ZGLX_MACRO_STR_H */
