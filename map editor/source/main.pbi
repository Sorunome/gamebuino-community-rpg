#WINDOW_WIDTH = 800
#WINDOW_HEIGHT = 500
#TILE_WIDTH = 8
#TILE_HEIGHT = 8
#TILES_ZOOM = 3
; #CONFIG_FILE = "data.ini"
; #SETTINGS_FILE = "settings.inc"

CompilerIf #PB_Compiler_OS = #PB_OS_Windows
  #Path_separator = "\"
  #StatusBarFirstFieldWidth = 50
CompilerElse
  #Path_separator = "/"
  #StatusBarFirstFieldWidth = 65
CompilerEndIf

Enumeration
  #Window
  #Window_new
EndEnumeration

; Structure s_project
;   new_game_map.s
;   x.l
;   y.l
;   offset_x.l
;   offset_y.l
;   direction.l
; EndStructure

Structure s_map
  title.s
  file.s
  w.b
  h.b
EndStructure

Structure s_map_tile
  id.b
EndStructure

Structure s_map_cursor
  x.b
  y.b
EndStructure

; Global project.s_project
Global isset.l = 0 ; 0 : no current map, 1 : unsaved map, 2 : saved map
Global drawing_allowed.l = 0
Global tool.l = 1
Global zoom.l
Global grid.l
Global map_.s_map
Global tiles_cursor.s_map_tile
tiles_cursor\id = 0
Global map_cursor.s_map_cursor
map_cursor\x = -2
map_cursor\y = -2

Declare Main()
Declare Close()

XIncludeFile "init.pb"
XIncludeFile "new.pb"
XIncludeFile "save.pb"
XIncludeFile "open.pb"
XIncludeFile "export.pb"
XIncludeFile "map.pb"
XIncludeFile "parameters.pb"
; XIncludeFile "project_parameters.pb"
; IDE Options = PureBasic 5.30 (Windows - x86)
; CursorPosition = 46
; FirstLine = 42
; UseMainFile = main.pb
; CurrentDirectory = ..\
; EnablePurifier