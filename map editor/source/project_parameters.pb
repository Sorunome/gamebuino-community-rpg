XIncludeFile "project_parameters.pbi"

Procedure Project_parameters()
  If OpenWindow(#Window_new, 0, 0, #ProjectParameters_w, #ProjectParameters_h, "Project parameters", #PB_Window_WindowCentered | #PB_Window_TitleBar, WindowID(#Window))
    Frame3DGadget(#NewGameFrame, #NewGameFrame_x, #NewGameFrame_y, #NewGameFrame_w, #NewGameFrame_h, "New game settings")
    TextGadget(#NewGameMapText, #NewGameMapText_x, #NewGameMapText_y, #NewGameMapText_w, #NewGameMapText_h, "New game map :")
    StringGadget(#NewGameMapString, #NewGameMapString_x, #NewGameMapString_y, #NewGameMapString_w, #NewGameMapString_h, "", #PB_String_ReadOnly)  
    SetGadgetText(#NewGameMapString, project\new_game_map)
    ButtonGadget(#NewGameMapButton, #NewGameMapButton_x, #NewGameMapButton_y, #NewGameMapButton_w, #NewGameMapButton_h, "...")
    TextGadget(#NewGameXText, #NewGameXText_x, #NewGameXText_y, #NewGameXText_w, #NewGameXText_h, "X =", #PB_Text_Right)
    SpinGadget(#NewGameXSpin, #NewGameXSpin_x, #NewGameXSpin_y, #NewGameXSpin_w, #NewGameXSpin_h, 0, 11, #PB_Spin_ReadOnly|#PB_Spin_Numeric)
    SetGadgetState(#NewGameXSpin, project\x)
    TextGadget(#NewGameYText, #NewGameYText_x, #NewGameYText_y, #NewGameYText_w, #NewGameYText_h, "Y =", #PB_Text_Right)
    SpinGadget(#NewGameYSpin, #NewGameYSpin_x, #NewGameYSpin_y, #NewGameYSpin_w, #NewGameYSpin_h, 0, 7, #PB_Spin_ReadOnly|#PB_Spin_Numeric)
    SetGadgetState(#NewGameYSpin, project\y)
    TextGadget(#NewGameOffsetXText, #NewGameOffsetXText_x, #NewGameOffsetXText_y, #NewGameOffsetXText_w, #NewGameOffsetXText_h, "Offset X =", #PB_Text_Right)
    SpinGadget(#NewGameOffsetXSpin, #NewGameOffsetXSpin_x, #NewGameOffsetXSpin_y, #NewGameOffsetXSpin_w, #NewGameOffsetXSpin_h, 0, 20, #PB_Spin_ReadOnly|#PB_Spin_Numeric)
    SetGadgetState(#NewGameOffsetXSpin, project\offset_x)
    TextGadget(#NewGameOffsetYText, #NewGameOffsetYText_x, #NewGameOffsetYText_y, #NewGameOffsetYText_w, #NewGameOffsetYText_h, "Offset Y =", #PB_Text_Right)
    SpinGadget(#NewGameOffsetYSpin, #NewGameOffsetYSpin_x, #NewGameOffsetYSpin_y, #NewGameOffsetYSpin_w, #NewGameOffsetYSpin_h, 0, 24, #PB_Spin_ReadOnly|#PB_Spin_Numeric)
    SetGadgetState(#NewGameOffsetYSpin, project\offset_y)
    TextGadget(#NewGameDirectionText, #NewGameDirectionText_x, #NewGameDirectionText_y, #NewGameDirectionText_w, #NewGameDirectionText_h, "Player direction :")
    ComboBoxGadget(#NewGameDirectionComboBox, #NewGameDirectionComboBox_x, #NewGameDirectionComboBox_y, #NewGameDirectionComboBox_w, #NewGameDirectionComboBox_h, #PB_ComboBox_Image)
    AddGadgetItem(#NewGameDirectionComboBox, -1, "Down", ImageID(CatchImage(#PB_Any, ?Player_down_icon)))
    AddGadgetItem(#NewGameDirectionComboBox, -1, "Left", ImageID(CatchImage(#PB_Any, ?Player_left_icon)))
    AddGadgetItem(#NewGameDirectionComboBox, -1, "Up", ImageID(CatchImage(#PB_Any, ?Player_up_icon)))
    AddGadgetItem(#NewGameDirectionComboBox, -1, "Right", ImageID(CatchImage(#PB_Any, ?Player_right_icon)))
    SetGadgetState(#NewGameDirectionComboBox, 0)
    ButtonGadget(#OKButton2, #OKButton2_x, #OKButton2_y, #OKButton2_w, #OKButton2_h, "Ok")
    ButtonGadget(#CancelButton2, #CancelButton2_x, #CancelButton2_y, #CancelButton2_w, #CancelButton2_h, "Cancel")
    Protected ok.l = 0, quit.l = 0, map_path.s = "map"+#Path_separator, filterM.s = "Map (*.map)|*.map", map_.s
    DisableWindow(#Window, 1)
    Repeat
      Select WaitWindowEvent()
        Case #PB_Event_Gadget
          Select EventGadget()
            Case #NewGameMapButton
              SetGadgetText(#NewGameMapString, map_path+GetFilePart(OpenFileRequester("New game map...", map_path, filterM, 0)))
            Case #OKButton2
              ok = 1
              quit = 1
            Case #CancelButton2
              ok = 0
              quit = 1
          EndSelect
      EndSelect
      If GetGadgetText(#NewGameMapString) <> ""
        DisableGadget(#OKButton2, 0)
      Else
        DisableGadget(#OKButton2, 1)
      EndIf
    Until quit
    If ok
      project\new_game_map = GetGadgetText(#NewGameMapString)
      project\x = GetGadgetState(#NewGameXSpin)
      project\y = GetGadgetState(#NewGameYSpin)
      project\offset_x = GetGadgetState(#NewGameOffsetXSpin)
      project\offset_y = GetGadgetState(#NewGameOffsetYSpin)
      project\direction = GetGadgetState(#NewGameDirectionComboBox)
      If OpenPreferences(#CONFIG_FILE)
        If PreferenceGroup("Project")
          WritePreferenceString("New game map", project\new_game_map)
          WritePreferenceLong("New game X", project\x)
          WritePreferenceLong("New game Y", project\y)
          WritePreferenceLong("New game offset X", project\offset_x)
          WritePreferenceLong("New game offset Y", project\offset_y)
          WritePreferenceLong("New game direction", project\direction)
        EndIf
        ClosePreferences()
      EndIf
      If CreateFile(0, #SETTINGS_FILE)
        WriteStringN(0, "#define new_game_x"+Chr(9)+Str(project\x*#TILE_WIDTH))
        WriteStringN(0, "#define new_game_y"+Chr(9)+Str(project\y*#TILE_HEIGHT))
        WriteStringN(0, "#define new_game_offset_x"+Chr(9)+Str(project\offset_x*#TILE_WIDTH))
        WriteStringN(0, "#define new_game_offset_y"+Chr(9)+Str(project\offset_y*#TILE_HEIGHT))
        If ReadFile(1, project\new_game_map)
          WriteStringN(0, "#define new_game_map"+Chr(9)+"map_"+ReadString(1))
          CloseFile(1)
        EndIf
        Select project\direction
          Case 0
            WriteString(0, "#define new_game_direction"+Chr(9)+"player_down")
          Case 1
            WriteString(0, "#define new_game_direction"+Chr(9)+"(player_left)")
          Case 2
            WriteString(0, "#define new_game_direction"+Chr(9)+"player_up")
          Case 3
            WriteString(0, "#define new_game_direction"+Chr(9)+"(player_right)")
        EndSelect
        CloseFile(0)
      EndIf
    EndIf
    CloseWindow(#Window_new)
    DisableWindow(#Window, 0)
    SetActiveWindow(#Window)
  EndIf
EndProcedure
; IDE Options = PureBasic 5.11 (Windows - x86)
; CursorPosition = 4
; Folding = -
; EnableXP
; UseMainFile = main.pb
; CurrentDirectory = ..\
; EnablePurifier