XIncludeFile "init.pbi"

Procedure Init()
  UsePNGImageDecoder()
  UsePNGImageEncoder()
  
  If OpenWindow(#Window, 0, 0, #WINDOW_WIDTH, #WINDOW_HEIGHT, "Map editor v"+#VERSION, #PB_Window_SystemMenu | #PB_Window_MinimizeGadget | #PB_Window_MaximizeGadget | #PB_Window_SizeGadget | #PB_Window_ScreenCentered | #PB_Window_TitleBar)=0
    MessageRequester("OpenWindow() error", "Can't open editor.")
    End
  EndIf
  
  WindowBounds(#Window, #WINDOW_WIDTH, #WINDOW_HEIGHT, #PB_Ignore, #PB_Ignore)
  
  If LoadFont(0, "Tahoma", 8, #PB_Font_HighQuality)
    SetGadgetFont(#PB_Default, FontID(0))
  EndIf
  
  If CreateImageMenu(#Menu, WindowID(#Window), #PB_Menu_ModernLook)
    MenuTitle("File")
    MenuItem(#New,"New", ImageID(CatchImage(#PB_Any, ?New_icon)))
    MenuItem(#Open,"Open...", ImageID(CatchImage(#PB_Any, ?Open_icon)))
    MenuItem(#Save,"Save", ImageID(CatchImage(#PB_Any, ?Save_icon)))
    MenuItem(#Save_as,"Save as...", ImageID(CatchImage(#PB_Any, ?Save_as_icon)))
    MenuItem(#Export,"Export...", ImageID(CatchImage(#PB_Any, ?Export_icon)))
    MenuItem(#Export_as,"Export as...", ImageID(CatchImage(#PB_Any, ?Export_as_icon)))
    MenuItem(#Close,"Close", ImageID(CatchImage(#PB_Any, ?Close_icon)))
    MenuBar()
    MenuItem(#Quit,"Quit", ImageID(CatchImage(#PB_Any, ?Quit_icon)))
    MenuTitle("Tools")
    MenuItem(#Pen, "Pen", ImageID(CatchImage(#PB_Any, ?Pen_icon)))
    MenuItem(#Square, "Square", ImageID(CatchImage(#PB_Any, ?Square_icon)))
    MenuItem(#Fill, "Fill", ImageID(CatchImage(#PB_Any, ?Fill_icon)))
    MenuBar()
    MenuItem(#Zoom1, "Zoom*1", ImageID(CatchImage(#PB_Any, ?Zoom1_icon)))
    MenuItem(#Zoom2, "Zoom*2", ImageID(CatchImage(#PB_Any, ?Zoom2_icon)))
    MenuItem(#Zoom3, "Zoom*3", ImageID(CatchImage(#PB_Any, ?Zoom3_icon)))
    MenuItem(#Grid, "Grid", ImageID(CatchImage(#PB_Any, ?Grid_icon)))
    MenuBar()
    MenuItem(#Parameters, "Map parameters", ImageID(CatchImage(#PB_Any, ?Parameters_icon)))
    MenuItem(#Screenshot,"Screenshot", ImageID(CatchImage(#PB_Any, ?Screenshot_icon)))
;     MenuBar()
;     MenuItem(#Project_parameters, "Project parameters", ImageID(CatchImage(#PB_Any, ?Project_parameters_icon)))
    MenuTitle("Help")
    MenuItem(#About,"About...", ImageID(CatchImage(#PB_Any, ?Help_icon)))
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
  EndIf
  
  If CreateToolBar(#ToolBar, WindowID(#Window))
    ToolBarImageButton(#New, ImageID(CatchImage(#PB_Any, ?New_icon)))
    ToolBarImageButton(#Open, ImageID(CatchImage(#PB_Any, ?Open_icon)))
    ToolBarImageButton(#Save, ImageID(CatchImage(#PB_Any, ?Save_icon)))
    ToolBarImageButton(#Save_as, ImageID(CatchImage(#PB_Any, ?Save_as_icon)))
    ToolBarImageButton(#Export, ImageID(CatchImage(#PB_Any, ?Export_icon)))
    ToolBarImageButton(#Export_as, ImageID(CatchImage(#PB_Any, ?Export_as_icon)))
    ToolBarImageButton(#Close, ImageID(CatchImage(#PB_Any, ?Close_icon)))
    ToolBarSeparator()
    ToolBarImageButton(#Pen, ImageID(CatchImage(#PB_Any, ?Pen_icon)), #PB_ToolBar_Toggle)
    ToolBarImageButton(#Square, ImageID(CatchImage(#PB_Any, ?Square_icon)), #PB_ToolBar_Toggle)
    ToolBarImageButton(#Fill, ImageID(CatchImage(#PB_Any, ?Fill_icon)), #PB_ToolBar_Toggle)
    ToolBarSeparator()
    ToolBarImageButton(#Zoom1, ImageID(CatchImage(#PB_Any, ?Zoom1_icon)), #PB_ToolBar_Toggle)
    ToolBarImageButton(#Zoom2, ImageID(CatchImage(#PB_Any, ?Zoom2_icon)), #PB_ToolBar_Toggle)
    ToolBarImageButton(#Zoom3, ImageID(CatchImage(#PB_Any, ?Zoom3_icon)), #PB_ToolBar_Toggle)
    ToolBarImageButton(#Grid, ImageID(CatchImage(#PB_Any, ?Grid_icon)), #PB_ToolBar_Toggle)
    ToolBarSeparator()
    ToolBarImageButton(#Parameters, ImageID(CatchImage(#PB_Any, ?Parameters_icon)))
    ToolBarImageButton(#Screenshot, ImageID(CatchImage(#PB_Any, ?Screenshot_icon)))
;     ToolBarSeparator()
;     ToolBarImageButton(#Project_parameters, ImageID(CatchImage(#PB_Any, ?Project_parameters_icon)))
    ToolBarToolTip(#ToolBar, #New, "New")
    ToolBarToolTip(#ToolBar, #Open, "Open...")
    ToolBarToolTip(#ToolBar, #Save, "Save")
    ToolBarToolTip(#ToolBar, #Save_as, "Save as...")
    ToolBarToolTip(#ToolBar, #Export, "Export...")
    ToolBarToolTip(#ToolBar, #Export_as, "Export as...")
    ToolBarToolTip(#ToolBar, #Close, "Close")
    ToolBarToolTip(#ToolBar, #Pen, "Pen")
    ToolBarToolTip(#ToolBar, #Square, "Square")
    ToolBarToolTip(#ToolBar, #Fill, "Fill")
    ToolBarToolTip(#ToolBar, #Zoom1, "Zoom*1")
    ToolBarToolTip(#ToolBar, #Zoom2, "Zoom*2")
    ToolBarToolTip(#ToolBar, #Zoom3, "Zoom*3")
    ToolBarToolTip(#ToolBar, #Grid, "Grid")
    ToolBarToolTip(#ToolBar, #Parameters, "Map parameters")
    ToolBarToolTip(#ToolBar, #Screenshot, "Screenshot")
;     ToolBarToolTip(#ToolBar, #Project_parameters, "Project parameters")
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
  EndIf
  
;   If OpenPreferences(#CONFIG_FILE) = 0
;     CreatePreferences(#CONFIG_FILE)
;     PreferenceGroup("Project")
;     PreferenceComment("Project settings")
;     WritePreferenceString("New game map", "")
;     WritePreferenceLong("New game X", 0)
;     WritePreferenceLong("New game Y", 0)
;     WritePreferenceLong("New game offset X", 0)
;     WritePreferenceLong("New game offset Y", 0)
;     WritePreferenceLong("New game direction", 0)
;   EndIf
  
;   Protected i.l = 0
;   If PreferenceGroup("Project")
;     project\new_game_map = ReadPreferenceString("New game map", "")
;     project\x = ReadPreferenceLong("New game X", 0)
;     project\y = ReadPreferenceLong("New game Y", 0)
;     project\offset_x = ReadPreferenceLong("New game offset X", 0)
;     project\offset_y = ReadPreferenceLong("New game offset Y", 0)
;     project\direction = ReadPreferenceLong("New game direction", 0)
;   EndIf

;   ClosePreferences()

  If LoadImage(#TilesImage, "tiles"+#Path_separator+"tiles.bmp") = 0
    MessageRequester("LoadImage() error", "Can't locate "+Chr(34)+"tiles"+#Path_separator+"tiles.bmp"+Chr(34))
    End
  EndIf
EndProcedure

Procedure InitWorkArea()
  If CreateStatusBar(#StatusBar, WindowID(#Window))
    AddStatusBarField(#StatusBarFirstFieldWidth)
    AddStatusBarField(25)
    AddStatusBarField(#PB_Ignore)
  EndIf
  
  CreateImage(#MapImage, map_\w*#TILE_WIDTH, map_\h*#TILE_HEIGHT)
  
  DisableMenuItem(#Menu, #Save_as, 0)
  DisableMenuItem(#Menu, #Export, 0)
  DisableMenuItem(#Menu, #Export_as, 0)
  DisableMenuItem(#Menu, #Close, 0)
  DisableMenuItem(#Menu, #Pen, 0)
  DisableMenuItem(#Menu, #Square, 0)
  DisableMenuItem(#Menu, #Fill, 0)
  DisableMenuItem(#Menu, #Zoom1, 0)
  DisableMenuItem(#Menu, #Zoom2, 0)
  DisableMenuItem(#Menu, #Zoom3, 0)
  DisableMenuItem(#Menu, #Grid, 0)
  DisableMenuItem(#Menu, #Parameters, 0)
  DisableMenuItem(#Menu, #Screenshot, 0)
  DisableToolBarButton(#ToolBar, #Save_as, 0)
  DisableToolBarButton(#ToolBar, #Export, 0)
  DisableToolBarButton(#ToolBar, #Export_as, 0)
  DisableToolBarButton(#ToolBar, #Close, 0)
  DisableToolBarButton(#ToolBar, #Pen, 0)
  DisableToolBarButton(#ToolBar, #Square, 0)
  DisableToolBarButton(#ToolBar, #Fill, 0)
  DisableToolBarButton(#ToolBar, #Zoom1, 0)
  DisableToolBarButton(#ToolBar, #Zoom2, 0)
  DisableToolBarButton(#ToolBar, #Zoom3, 0)
  DisableToolBarButton(#ToolBar, #Grid, 0)
   DisableToolBarButton(#ToolBar, #Parameters, 0)
  DisableToolBarButton(#ToolBar, #Screenshot, 0)
  SetToolBarButtonState(#ToolBar, #Pen, 1)
  SetToolBarButtonState(#ToolBar, #Zoom1, 1)
  tool = 1
  zoom = 1
  grid = 0
  
  UseGadgetList(WindowID(#Window))
  
  ScrollAreaGadget(#TilesScrollArea, #TilesScrollArea_x, #TilesScrollArea_y, 0, 0, ImageWidth(#TilesImage)*#TILES_ZOOM, ImageHeight(#TilesImage)*#TILES_ZOOM, 1, #PB_ScrollArea_Single)
  CanvasGadget(#TilesGadget, 0, 0, ImageWidth(#TilesImage)*#TILES_ZOOM, ImageHeight(#TilesImage)*#TILES_ZOOM) 
  CloseGadgetList()  
  
  ScrollAreaGadget(#MapScrollArea, #MapScrollArea_x, #MapScrollArea_y, 0, 0, ImageWidth(#MapImage), ImageHeight(#MapImage), 1, #PB_ScrollArea_Single|#PB_ScrollArea_Center)
  CanvasGadget(#MapGadget, 0, 0, ImageWidth(#MapImage), ImageHeight(#MapImage))
  CloseGadgetList()

  ResizeWindow_()
EndProcedure

Procedure ResizeWindow_()
  ResizeGadget(#TilesScrollArea, #TilesScrollArea_x, #TilesScrollArea_y, #TilesScrollArea_w, WindowHeight(#Window)-#TilesScrollArea_y-44)
  ResizeGadget(#MapScrollArea, #MapScrollArea_x, #MapScrollArea_y, WindowWidth(#Window)-#MapScrollArea_x-1, WindowHeight(#Window)-#MapScrollArea_y-44)
EndProcedure

DataSection
  New_icon:
  IncludeBinary "icons"+#Path_separator+"new.png"
  Open_icon:
  IncludeBinary "icons"+#Path_separator+"open.png"
  Save_icon:
  IncludeBinary "icons"+#Path_separator+"save.png"
  Save_as_icon:
  IncludeBinary "icons"+#Path_separator+"save_as.png"
  Export_icon:
  IncludeBinary "icons"+#Path_separator+"export.png"
  Export_as_icon:
  IncludeBinary "icons"+#Path_separator+"export_as.png"
  Close_icon:
  IncludeBinary "icons"+#Path_separator+"close.png"
  Quit_icon:
  IncludeBinary "icons"+#Path_separator+"quit.png"
  Pen_icon:
  IncludeBinary "icons"+#Path_separator+"pen.png"
  Square_icon:
  IncludeBinary "icons"+#Path_separator+"square.png"
  Fill_icon:
  IncludeBinary "icons"+#Path_separator+"fill.png"
  Zoom1_icon:
  IncludeBinary "icons"+#Path_separator+"zoom1.png"
  Zoom2_icon:
  IncludeBinary "icons"+#Path_separator+"zoom2.png"
  Zoom3_icon:
  IncludeBinary "icons"+#Path_separator+"zoom3.png"
  Grid_icon:
  IncludeBinary "icons"+#Path_separator+"grid.png"
  Parameters_icon:
  IncludeBinary "icons"+#Path_separator+"edit.png"
  Screenshot_icon:
  IncludeBinary "icons"+#Path_separator+"screenshot.png"
;   Project_parameters_icon:
;   IncludeBinary "icons"+#Path_separator+"project_parameters.png"
  Help_icon:
  IncludeBinary "icons"+#Path_separator+"help.png"
EndDataSection
; IDE Options = PureBasic 5.30 (Windows - x86)
; CursorPosition = 80
; Folding = -
; UseMainFile = main.pb
; CurrentDirectory = ..\
; EnablePurifier