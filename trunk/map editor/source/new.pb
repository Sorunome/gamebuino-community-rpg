XIncludeFile "new.pbi"

Procedure New()
  If OpenWindow(#Window_new, 0, 0, #Window_new_w, #Window_new_h, "New map", #PB_Window_WindowCentered | #PB_Window_TitleBar, WindowID(#Window))
    Frame3DGadget(#TitleFrame, #TitleFrame_x, #TitleFrame_y, #TitleFrame_w, #TitleFrame_h, "Name :")
    StringGadget(#TitleInput, #TitleInput_x, #TitleInput_y, #TitleInput_w, #TitleInput_h, "")
    Frame3DGadget(#DimensionsFrame, #DimensionsFrame_x, #DimensionsFrame_y, #DimensionsFrame_w, #DimensionsFrame_h, "Dimensions :")
    TextGadget(#TextWidth, #TextWidth_x, #TextWidth_y, #TextWidth_w, #TextWidth_h, "Width : ")
    SpinGadget(#SpinWidth, #SpinWidth_x, #SpinWidth_y, #SpinWidth_w, #SpinWidth_h, 0, 1000, #PB_Spin_Numeric)
    SetGadgetAttribute(#SpinWidth, #PB_Spin_Minimum, 12)
    SetGadgetAttribute(#SpinWidth, #PB_Spin_Maximum, 255)
    SetGadgetState(#SpinWidth, 12)
    TextGadget(#TextHeight, #TextHeight_x, #TextHeight_y, #TextHeight_w, #TextHeight_h, "Height : ")
    SpinGadget(#SpinHeight, #SpinHeight_x, #SpinHeight_y, #SpinHeight_w, #SpinHeight_h, 0, 1000, #PB_Spin_Numeric)
    SetGadgetAttribute(#SpinHeight, #PB_Spin_Minimum, 8)
    SetGadgetAttribute(#SpinHeight, #PB_Spin_Maximum, 255)
    SetGadgetState(#SpinHeight, 8)
    ButtonGadget(#OKButton, #OKButton_x, #OKButton_y, #OKButton_w, #OKButton_h, "Ok")
    ButtonGadget(#CancelButton, #CancelButton_x, #CancelButton_y, #CancelButton_w, #CancelButton_h, "Cancel")
    Protected ok.l = 0, quit.l = 0
    DisableWindow(#Window, 1)
    Repeat
      Select WaitWindowEvent()
        Case #PB_Event_Gadget
          Select EventGadget()
            Case #OKButton
              ok = 1
              quit = 1
            Case #CancelButton
              ok = 0
              quit = 1
          EndSelect
      EndSelect
      If GetGadgetText(#TitleInput) <> ""
        DisableGadget(#OKButton, 0)
      Else
        DisableGadget(#OKButton, 1)
      EndIf
    Until quit
    If ok
      isset = 1
      map_\title = GetGadgetText(#TitleInput)
      map_\file = ""
      map_\w = GetGadgetState(#SpinWidth)
      map_\h = GetGadgetState(#SpinHeight)
      Global Dim map_data.s_map_tile(map_\w-1, map_\h-1)
      Global Dim map_data_temp.s_map_tile(map_\w-1, map_\h-1)
      Global Dim map_data_temp2.s_map_tile(map_\w-1, map_\h-1)
    EndIf
    CloseWindow(#Window_new)
    DisableWindow(#Window, 0)
    SetActiveWindow(#Window)
  EndIf
EndProcedure
; IDE Options = PureBasic 5.11 (Windows - x86)
; CursorPosition = 39
; FirstLine = 24
; Folding = -
; UseMainFile = main.pb
; CurrentDirectory = ..\
; EnablePurifier