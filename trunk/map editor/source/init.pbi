#TilesScrollArea_x = 1
#TilesScrollArea_y = 28
#TilesScrollArea_w = #TILE_WIDTH*#TILES_ZOOM+2+18
#MapScrollArea_x = #TilesScrollArea_x+#TilesScrollArea_w+1
#MapScrollArea_y = #TilesScrollArea_y
#SplitterGadget_x = #TilesScrollArea_x-1

Enumeration
  #Menu
  ;
  #New
  #Open
  #Save
  #Save_as
  #Export
  #Export_as
  #Close
  #Quit
  #Grid
  #Parameters
  #Screenshot
;   #Project_parameters
  #About
  ;
  #ToolBar
  #Pen
  #Square
  #Fill
  #Zoom1
  #Zoom2
  #Zoom3
  ;
  #TilesScrollArea
  #TilesGadget
  #MapScrollArea
  #MapGadget
EndEnumeration

Enumeration
  #StatusBar
  #StatusBar2
EndEnumeration

Enumeration 8000
  #MapImage
  #MapImage_temp
  #TilesImage
  #TilesImage_temp
  #TileImage
EndEnumeration

Declare Init()
Declare InitWorkArea()
Declare ResizeWindow_()
; IDE Options = PureBasic 5.30 (Windows - x86)
; CursorPosition = 21
; UseMainFile = main.pb
; CurrentDirectory = ..\
; EnablePurifier