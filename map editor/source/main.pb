;Map editor - deeph copyleft 2016
#VERSION = "0.1"

EnableExplicit
XIncludeFile "main.pbi"
Init()
Main()
End

Procedure Main()
  If CountProgramParameters() > 0
    Open(ProgramParameter())
    If isset > 0
      InitWorkArea()
      HandleTiles()
      HandleMap()
    EndIf
  EndIf
  Protected event.l, quit.l = 0, ans.l, file.s, map_path.s = "maps"+#Path_separator, filterM.s = "Map (*.map)|*.map", mapx_path.s = "map"+#Path_separator, filterMx.s = "data (*.DAT)|*.DAT", x.l, y.l, ok.l = 0
  Repeat
    Repeat
      event = WindowEvent()
      If EventWindow() = #Window
        Select event
          Case #PB_Event_Menu
            drawing_allowed = 0
            Select EventMenu()
              Case #New
                If isset = 1
                  ans = MessageRequester("Error", "Current map isn't saved, do it now ?", #PB_MessageRequester_YesNoCancel)
                  If ans <> #PB_MessageRequester_Cancel
                    If ans = #PB_MessageRequester_Yes
                      If map_\file <> ""
                        Save(map_\file)
                      Else
                        SaveRequester(SaveFileRequester("Save as...", map_path, filterM, 0))
                      EndIf
                    EndIf
                    Close()
                    New()
                  EndIf
                Else
                  New()
                EndIf
                If isset = 1
                  InitWorkArea()
                  HandleTiles()
                  HandleMap()
                EndIf
              Case #Open
                file = OpenFileRequester("Open...", map_path, filterM, 0)
                If file
                  If FileSize(file) > 0
                    If isset = 1
                      If MessageRequester("Error", "Current map isn't saved. Do it now ?", #PB_MessageRequester_YesNo) = #PB_MessageRequester_Yes
                        SaveRequester(SaveFileRequester("Save as...", map_path, filterM, 0))
                      EndIf
                    EndIf
                    If isset > 0
                      Close()
                    EndIf
                    Open(file)
                    isset = 2
                    InitWorkArea()
                    HandleTiles()
                    HandleMap()
                  Else
                    MessageRequester("Erreur FileSize()", "Impossible d'ouvrir le fichier.")
                  EndIf
                EndIf
              Case #Save
                If map_\file <> ""
                  Save(map_\file)
                Else
                  SaveRequester(SaveFileRequester("Save as...", map_path, filterM, 0))
                EndIf
              Case #Save_as
                SaveRequester(SaveFileRequester("Save as...", map_path, filterM, 0))
              Case #Export
                ExportMap(map_path+UCase(map_\title)+".DAT")
              Case #Export_as
                ExportRequester(SaveFileRequester("Export as...", mapx_path, filterMx, 0))
              Case #Close
                If isset = 1
                  If MessageRequester("Error", "Current map isn't saved. Do it now ?", #PB_MessageRequester_YesNo) = #PB_MessageRequester_Yes
                    If map_\file <> ""
                      Save(UCase(map_\file))
                    Else
                      SaveRequester(SaveFileRequester("Save as...", map_path, filterM, 0))
                    EndIf
                  EndIf
                EndIf
                Close()
                isset = 0
              Case #Zoom1
                zoom = 1
                SetToolBarButtonState(#ToolBar, #Zoom1, 1)
                SetToolBarButtonState(#ToolBar, #Zoom2, 0)
                SetToolBarButtonState(#ToolBar, #Zoom3, 0)
                SetGadgetAttribute(#MapScrollArea, #PB_ScrollArea_InnerWidth, map_\w*#TILE_WIDTH*zoom)
                SetGadgetAttribute(#MapScrollArea, #PB_ScrollArea_InnerHeight, map_\h*#TILE_HEIGHT*zoom)
                ResizeGadget(#MapGadget, #PB_Ignore, #PB_Ignore, map_\w*#TILE_WIDTH*zoom, map_\h*#TILE_HEIGHT*zoom)
                RefreshMap()
              Case #Zoom2
                zoom = 2
                SetToolBarButtonState(#ToolBar, #Zoom1, 0)
                SetToolBarButtonState(#ToolBar, #Zoom2, 1)
                SetToolBarButtonState(#ToolBar, #Zoom3, 0)
                SetGadgetAttribute(#MapScrollArea, #PB_ScrollArea_InnerWidth, map_\w*#TILE_WIDTH*zoom)
                SetGadgetAttribute(#MapScrollArea, #PB_ScrollArea_InnerHeight, map_\h*#TILE_HEIGHT*zoom)
                ResizeGadget(#MapGadget, #PB_Ignore, #PB_Ignore, map_\w*#TILE_WIDTH*zoom, map_\h*#TILE_HEIGHT*zoom)
                RefreshMap()
              Case #Zoom3
                zoom = 3
                SetToolBarButtonState(#ToolBar, #Zoom1, 0)
                SetToolBarButtonState(#ToolBar, #Zoom2, 0)
                SetToolBarButtonState(#ToolBar, #Zoom3, 1)
                SetGadgetAttribute(#MapScrollArea, #PB_ScrollArea_InnerWidth, map_\w*#TILE_WIDTH*zoom)
                SetGadgetAttribute(#MapScrollArea, #PB_ScrollArea_InnerHeight, map_\h*#TILE_HEIGHT*zoom)
                ResizeGadget(#MapGadget, #PB_Ignore, #PB_Ignore, map_\w*#TILE_WIDTH*zoom, map_\h*#TILE_HEIGHT*zoom)
                RefreshMap()
              Case #Quit
                quit = 1
              Case #Grid
                grid+1
                If grid = 2
                  grid = 0
                EndIf
                SetMenuItemState(#Menu, #Grid, grid)
                RefreshMap()
              Case #Parameters
                Parameters()
              Case #Screenshot
                Screenshot()
;               Case #Project_parameters
;                 Project_parameters()
              Case #About
                MessageRequester("About...", "Map editor v"+#VERSION+" - "+FormatDate("%dd/%mm/%yy %hhh%ii", #PB_Compiler_Date)+" - "+"deeph copyleft 2016")
              Case #Pen
                SetToolBarButtonState(#ToolBar, #Pen, 1)
                SetToolBarButtonState(#ToolBar, #Square, 0)
                SetToolBarButtonState(#ToolBar, #Fill, 0)
                tool = 1
              Case #Square
                SetToolBarButtonState(#ToolBar, #Pen, 0)
                SetToolBarButtonState(#ToolBar, #Square, 1)
                SetToolBarButtonState(#ToolBar, #Fill, 0)
                tool = 2
              Case #Fill
                SetToolBarButtonState(#ToolBar, #Pen, 0)
                SetToolBarButtonState(#ToolBar, #Square, 0)
                SetToolBarButtonState(#ToolBar, #Fill, 1)
                tool = 3
            EndSelect
          Case #PB_Event_Gadget
            Select EventGadget()
              Case #MapGadget
                Select EventType()
                  Case #PB_EventType_MouseEnter
                    drawing_allowed = 1
                  Case #PB_EventType_MouseLeave
                    drawing_allowed = 0
                EndSelect
                MapEvent(event)
              Case #TilesGadget
                Select EventType()
                  Case #PB_EventType_MouseEnter
                    drawing_allowed = 1
                  Case #PB_EventType_MouseLeave
                    drawing_allowed = 0
                EndSelect
                TilesEvent(event)
            EndSelect
          Case #PB_Event_SizeWindow
            drawing_allowed = 0
            If isset > 0
              ResizeWindow_()
            EndIf
          Case #PB_Event_CloseWindow
            quit = 1
          Case #PB_Event_ActivateWindow
            drawing_allowed = 0
        EndSelect
        If isset = 1
          DisableMenuItem(#Menu, #Save, 0)
          DisableToolBarButton(#ToolBar, #Save, 0)
        ElseIf isset = 2
          DisableMenuItem(#Menu, #Save, 1)
          DisableToolBarButton(#ToolBar, #Save, 1)
        EndIf
      EndIf
    Until event = 0
    Delay(10)
  Until quit
  If isset = 1
    If isset = 1
      If MessageRequester("Error", "Current map isn't saved. Do it now ?", #PB_MessageRequester_YesNo) = #PB_MessageRequester_Yes
        If map_\file <> ""
          Save(map_\file)
        Else
          SaveRequester(SaveFileRequester("Save as...", map_path, filterM, 0))
        EndIf
      EndIf
    EndIf
  EndIf
EndProcedure

Procedure Close()
  FreeStatusBar(#StatusBar)
  DisableMenuItem(#Menu, #Save, 1)
  DisableMenuItem(#Menu, #Save_as, 1)
  DisableMenuItem(#Menu, #Export, 1)
  DisableMenuItem(#Menu, #Export_as, 1)
  DisableMenuItem(#Menu, #Close, 1)
  DisableMenuItem(#Menu, #Pen, 1)
  DisableMenuItem(#Menu, #Square, 1)
  DisableMenuItem(#Menu, #Fill, 1)
  DisableMenuItem(#Menu, #Zoom1, 1)
  DisableMenuItem(#Menu, #Zoom2, 1)
  DisableMenuItem(#Menu, #Zoom3, 1)
  DisableMenuItem(#Menu, #Grid, 1)
  DisableMenuItem(#Menu, #Parameters, 1)
  DisableMenuItem(#Menu, #Screenshot, 1)
  DisableToolBarButton(#ToolBar, #Save, 1)
  DisableToolBarButton(#ToolBar, #Save_as, 1)
  DisableToolBarButton(#ToolBar, #Export, 1)
  DisableToolBarButton(#ToolBar, #Export_as, 1)
  DisableToolBarButton(#ToolBar, #Close, 1)
  DisableToolBarButton(#ToolBar, #Pen, 1)
  DisableToolBarButton(#ToolBar, #Square, 1)
  DisableToolBarButton(#ToolBar, #Fill, 1)
  DisableToolBarButton(#ToolBar, #Zoom1, 1)
  DisableToolBarButton(#ToolBar, #Zoom2, 1)
  DisableToolBarButton(#ToolBar, #Zoom3, 1)
  DisableToolBarButton(#ToolBar, #Grid, 1)
  DisableToolBarButton(#ToolBar, #Parameters, 1)
  DisableToolBarButton(#ToolBar, #Screenshot, 1)
  SetToolBarButtonState(#ToolBar, #Zoom1, 0)
  SetToolBarButtonState(#ToolBar, #Zoom2, 0)
  SetToolBarButtonState(#ToolBar, #Zoom3, 0)
  FreeGadget(#TilesGadget)
  FreeGadget(#TilesScrollArea)
  FreeGadget(#MapGadget)
  FreeGadget(#MapScrollArea)
  isset = 0
EndProcedure
; IDE Options = PureBasic 5.30 (Windows - x86)
; CursorPosition = 72
; FirstLine = 58
; Folding = -
; EnableXP
; EnableUser
; UseIcon = icons\icon.ico
; Executable = ..\map editor.exe
; CurrentDirectory = ..\
; EnablePurifier
; IncludeVersionInfo
; VersionField0 = 0.0.0.1
; VersionField1 = 0.0.0.1
; VersionField2 = deeph
; VersionField3 = PokÃƒÆ’Ã†â€™Ãƒâ€ Ã¢â‚¬â„¢ÃƒÆ’Ã¢â‚¬Å¡Ãƒâ€šÃ‚Â©Maker
; VersionField6 = PokÃƒÆ’Ã†â€™Ãƒâ€ Ã¢â‚¬â„¢ÃƒÆ’Ã¢â‚¬Å¡Ãƒâ€šÃ‚Â©mon Monochrome editor
; VersionField9 = Copyleft
; VersionField14 = http://deeph.servhome.org
; VersionField16 = VFT_APP
; VersionField17 = 040c French (Standard)