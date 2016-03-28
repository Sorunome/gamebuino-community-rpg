XIncludeFile "map.pbi"

Macro Draw_box(x, y, w, h)
  LineXY(x, y, x+w, y, RGB(0, 0, 0))
  LineXY(x, y, x, y+h, RGB(0, 0, 0))
  LineXY(x+w, y+h, x+w, y, RGB(0, 0, 0))
  LineXY(x+w, y+h, x, y+h, RGB(0, 0, 0))
  ;
  LineXY(x+1, y+1, x+w-1, y+1, RGB(255, 255, 255))
  LineXY(x+1, y+1, x+1, y+h-1, RGB(255, 255, 255))
  LineXY(x+w-1, y+h-1, x+w-1, y+1, RGB(255, 255, 255))
  LineXY(x+w-1, y+h-1, x+1, y+h-1, RGB(255, 255, 255))
  ;
  LineXY(x+2, y+2, x+w-2, y+2, RGB(0, 0, 0))
  LineXY(x+2, y+2, x+2, y+h-2, RGB(0, 0, 0))
  LineXY(x+w-2, y+h-2, x+w-2, y+2, RGB(0, 0, 0))
  LineXY(x+w-2, y+h-2, x+2, y+h-2, RGB(0, 0, 0))
EndMacro

Procedure MapEvent(event.l)
  Protected tile_w.l, tile_h.l, x.l, y.l, s_x.l = 0, s_y.l = 0
  Protected x2.l, y2.l, start_x.l, end_x.l, start_y.l, end_y.l, clip_x.l, clip_y.l
  Static clic.l = 0, tool_x.l = -1, tool_y.l = -1, save_start_x.l, save_end_x.l, save_start_y.l, save_end_y.l
  map_cursor\x = Int(GetGadgetAttribute(#MapGadget, #PB_Canvas_MouseX)/(#TILE_WIDTH*zoom))
  map_cursor\y = Int(GetGadgetAttribute(#MapGadget, #PB_Canvas_MouseY)/(#TILE_HEIGHT*zoom))
  If drawing_allowed And map_cursor\x>=0 And map_cursor\x<map_\w And map_cursor\y>=0 And map_cursor\y<map_\h
    If EventType() = #PB_EventType_LeftButtonUp
      clic = 0
      map_cursor\x = -2
      map_cursor\y = -2
      If tool = 2 And tool_x <> -1 And tool_y <> -1
        tool_x = -1
        tool_y = -1
        save_start_x = 0
        save_start_y = 0
        save_end_x = 0
        save_end_y = 0
        CopyArray(map_data_temp2(), map_data())
      Else
        HandleTile(map_cursor\x, map_cursor\y)
      EndIf
      RefreshMap()
    ElseIf EventType() = #PB_EventType_LeftButtonDown Or clic = 1
      isset = 1
      StatusBarText(#StatusBar, 2, map_\title+"("+Str(map_cursor\x)+", "+Str(map_cursor\y)+")", #PB_StatusBar_BorderLess)
      clic = 1
      If tool = 1 ; Crayon
        map_data(map_cursor\x, map_cursor\y)\id = tiles_cursor\id
        HandleTile(map_cursor\x, map_cursor\y)
        RefreshMapWithCursor()
      ElseIf tool = 2 ; Rectangle
        If tool_x = -1 And tool_y = -1
          tool_x = map_cursor\x
          save_start_x = map_cursor\x
          save_end_x = map_cursor\x
          tool_y = map_cursor\y
          save_start_y = map_cursor\y
          save_end_y = map_cursor\y
        EndIf
        start_x = tool_x
        end_x = map_cursor\x
        start_y = tool_y
        end_y = map_cursor\y
        If start_x > end_x
          Swap start_x, end_x
        EndIf
        If start_y > end_y
          Swap start_y, end_y
        EndIf
        If save_start_x > start_x
          save_start_x = start_x
        EndIf
        If save_start_y > start_y
          save_start_y = start_y
        EndIf
        If save_end_x < end_x
          save_end_x = end_x
        EndIf
        If save_end_y < end_y
          save_end_y = end_y
        EndIf
        CopyArray(map_data(), map_data_temp())
        For x2 = start_x To end_x
          For y2 = start_y To end_y
            map_data(x2, y2)\id = tiles_cursor\id
          Next y2
        Next x2
        For x2 = save_start_x To save_end_x
          For y2 = save_start_y To save_end_y
            HandleTile(x2, y2)
          Next y2
        Next x2
        RefreshMapWithCursor()
        CopyArray(map_data(), map_data_temp2())
        CopyArray(map_data_temp(), map_data())
      ElseIf tool = 3 ; Remplissage
        floodFill4(map_cursor\x, map_cursor\y, tiles_cursor\id, map_data(map_cursor\x, map_cursor\y)\id)
        RefreshMapWithCursor()
      EndIf
    ElseIf EventType() = #PB_EventType_RightButtonUp
      RefreshMap()
    ElseIf EventType() = #PB_EventType_RightButtonDown
      tiles_cursor\id = map_data(map_cursor\x, map_cursor\y)\id
      HandleTiles()
      RefreshMapWithCursor()
    ElseIf EventType() = #PB_EventType_MouseMove
      StatusBarText(#StatusBar, 2, map_\title+"("+Str(map_cursor\x)+", "+Str(map_cursor\y)+")", #PB_StatusBar_BorderLess)
    EndIf
  Else
    clic = 0
    map_cursor\x = -2
    map_cursor\y = -2
    If tool = 2 And tool_x <> -1 And tool_y <> -1
      tool_x = -1
      tool_y = -1
      save_start_x = 0
      save_start_y = 0
      save_end_x = 0
      save_end_y = 0
      CopyArray(map_data_temp2(), map_data())
    EndIf
    RefreshMap()
  EndIf
EndProcedure

Procedure HandleTile(x.l, y.l)
  If x >= 0 And y >= 0 And x <= map_\w-1 And y <= map_\h-1
    If StartDrawing(ImageOutput(#MapImage))
      If GrabImage(#TilesImage, #TileImage, 0, map_data(x, y)\id*#TILE_HEIGHT, #TILE_WIDTH, #TILE_HEIGHT)
        DrawAlphaImage(ImageID(#TileImage), x*#TILE_WIDTH, y*#TILE_HEIGHT)
      EndIf
      StopDrawing()
    EndIf
  EndIf
EndProcedure

Procedure RefreshMapWithCursor()
  CopyImage(#MapImage, #MapImage_temp)
  ResizeImage(#MapImage_temp, map_\w*#TILE_WIDTH*zoom, map_\h*#TILE_HEIGHT*zoom, #PB_Image_Raw)
  Grid()
  HandleCursor()
  If StartDrawing(CanvasOutput(#MapGadget))
    DrawAlphaImage(ImageID(#MapImage_temp), 0, 0)
    StopDrawing()
  EndIf
EndProcedure

Procedure RefreshMap()
  CopyImage(#MapImage, #MapImage_temp)
  ResizeImage(#MapImage_temp, map_\w*#TILE_WIDTH*zoom, map_\h*#TILE_HEIGHT*zoom, #PB_Image_Raw)
  Grid()
  If StartDrawing(CanvasOutput(#MapGadget))
    DrawAlphaImage(ImageID(#MapImage_temp), 0, 0)
    StopDrawing()
  EndIf
EndProcedure

Procedure HandleMap()
  Protected x.l, y.l
  For x = 0 To map_\w-1
    For y = 0 To map_\h-1
      HandleTile(x, y)
    Next y
  Next x
  RefreshMap()
EndProcedure

Procedure HandleCursor()
  If StartDrawing(ImageOutput(#MapImage_temp))
    Draw_box(map_cursor\x*#TILE_WIDTH*zoom, map_cursor\y*#TILE_HEIGHT*zoom, #TILE_WIDTH*zoom, #TILE_HEIGHT*zoom)
    StopDrawing()
  EndIf
EndProcedure

Procedure Grid()
  Protected x.l, y.l
  If grid
    If StartDrawing(ImageOutput(#MapImage_temp))
      For x = #TILE_WIDTH*zoom To ImageWidth(#MapImage_temp)
        For y = #TILE_HEIGHT*zoom To ImageHeight(#MapImage_temp)
          LineXY(0, y, ImageWidth(#MapImage_temp), y, RGB(192, 192, 192))
          y+(#TILE_HEIGHT*zoom)-zoom
            y+zoom-1
        Next y
        LineXY(x, 0, x, ImageHeight(#MapImage_temp), RGB(192, 192, 192))
        x+(#TILE_WIDTH*zoom)-zoom
          x+zoom-1
      Next x
      StopDrawing()
    EndIf
  EndIf
EndProcedure

Procedure TilesEvent(event.l)
  Protected id.l
  id = Int(GetGadgetAttribute(#TilesGadget, #PB_Canvas_MouseY)/(#TILE_HEIGHT*#TILES_ZOOM))
  If EventType() = #PB_EventType_LeftClick And drawing_allowed
    tiles_cursor\id = id
    StatusBarText(#StatusBar, 0, "Tile #"+Str(tiles_cursor\id))
    If GrabImage(#TilesImage, #TileImage, 0, tiles_cursor\id*#TILE_HEIGHT, #TILE_WIDTH, #TILE_HEIGHT)
      If ResizeImage(#TileImage, #TILE_WIDTH*2, #TILE_HEIGHT*2, #PB_Image_Raw)
        StatusBarImage(#StatusBar, 1, ImageID(#TileImage))
      EndIf
    EndIf
    HandleTiles()
  EndIf
EndProcedure

Procedure HandleTiles()
  CopyImage(#TilesImage, #TilesImage_temp)
  ResizeImage(#TilesImage_temp, ImageWidth(#TilesImage_temp)*#TILES_ZOOM, ImageHeight(#TilesImage_temp)*#TILES_ZOOM, #PB_Image_Raw)
  If StartDrawing(CanvasOutput(#TilesGadget))
    DrawAlphaImage(ImageID(#TilesImage_temp), 0, 0)
    Draw_box(0, tiles_cursor\id*#TILE_HEIGHT*#TILES_ZOOM, #TILE_WIDTH*#TILES_ZOOM-1, #TILE_HEIGHT*#TILES_ZOOM-1)
    StopDrawing()
  EndIf
EndProcedure

Procedure floodFill4(x.l, y.l, new_tile_id.l, old_tile_id.l)
  If x >= 0 And x <= map_\w And y >= 0 And y <= map_\h And map_data(x, y)\id = old_tile_id And map_data(x, y)\id <> new_tile_id
    map_data(x, y)\id = new_tile_id
    HandleTile(x, y)
    floodFill4(x+1, y, new_tile_id, old_tile_id)
    floodFill4(x-1, y, new_tile_id, old_tile_id)
    floodFill4(x, y+1, new_tile_id, old_tile_id)
    floodFill4(x, y-1, new_tile_id, old_tile_id)
  EndIf
EndProcedure

Procedure Screenshot()
  Protected file.s = SaveFileRequester("Save as...", "", "Image (*.png)|*.png", 0)
  If file
    If FindString(file, ".png", 1) = 0
      file = file + ".png"
    EndIf
    If FileSize(file) > 0
      If MessageRequester("Error", "File already exists. Replace it ?", #PB_MessageRequester_YesNo) = #PB_MessageRequester_Yes
        DeleteFile(file)
      Else
        ProcedureReturn
      EndIf
    EndIf
    If SaveImage(#MapImage, file, #PB_ImagePlugin_PNG)
      CompilerIf #PB_Compiler_OS = #PB_OS_Windows
        sndPlaySound_("ding.wav", 0)
      CompilerEndIf
    EndIf
  EndIf
EndProcedure
; IDE Options = PureBasic 5.11 (Windows - x86)
; CursorPosition = 198
; FirstLine = 187
; Folding = ---
; UseMainFile = main.pb
; CurrentDirectory = ..\
; EnablePurifier