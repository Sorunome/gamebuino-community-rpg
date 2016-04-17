<?php
ini_set('display_errors',1);
ini_set('display_startup_errors',1);
error_reporting(E_ALL);
if(!$security->isLoggedIn() || !($user_info['power']&128)){
	echo $page->getPage('Nope','<script type="text/javascript">getPageJSON("/");</script>Redirecting...',$lang,$pathPartsParsed);
	die();
}
$sql->switchDb('soru_gamebuino_rpg');
include_once(realpath(dirname(__FILE__)).'/functions.php');
if(isset($_GET['get'])){
	$mask = rotate_buffer_left($_POST['buffer2']);
	$data = rotate_buffer_left($_POST['buffer1']);
	header('Content-Type: application/json');
	$hex = '';
	for($i = 0;$i < 16;$i +=2){
		$hex .= "0x".substr($mask,$i,2).",";
		$hex .= "0x".substr($data,$i,2).", ";
	}
	echo json_encode([
		'data' => $hex
	]);
	exit;
}
$html = '<h1>Masked sprite creator</h1>';
$html .= '<style type="text/css">
			#container {
				width:336px;
				padding:0;
				margin:0;
				line-height:0;
				display:inline-block;
			}
			#container2 {
				width:336px;
				padding:0;
				margin:0;
				line-height:0;
				display:inline-block;
			}
			.pixel {
				display:inline-block;
				width:40px;
				height:40px;
				padding:0;
				margin:0;
				border:1px solid #3D2F2F;
			}
			.pixel.color0 {
				background-color:#FFFFFF;
			}
			.pixel.color1 {
				background-color:#000000;
			}
			.pixel.color2,.pixel.color3 {
				background-color:transparent;
			}
		</style>
		<script type="text/javascript">
			function pad(n, width, z){
				z = z || "0";
				n = n + "";
				return n.length >= width ? n : new Array(width - n.length + 1).join(z) + n;
			}
			$(document).ready(function(){
				var spriteData = [],
					spriteData2 = [],
					i,j;
				for(i=0;i<8;i++){
					spriteData.push([0,0,0,0,0,0,0,0]);
					spriteData2.push([0,0,0,0,0,0,0,0]);
					for(j=0;j<8;j++){
						$("#container").append(
							$("<div>")
								.addClass("pixel")
								.addClass("color"+(spriteData[i][j] + 2*spriteData2[i][j]).toString(10))
								.data({
									i:i,
									j:j
								})
								.mousedown(function(e){
									i = $(this).data("i");
									j = $(this).data("j");
									$(this).removeClass(function (index, css) {
										return (css.match (/(^|\s)color\S+/g) || []).join(" ");
									});
									if(e.which == 1){ // left click
										e.preventDefault();
										
										spriteData[i][j] = Math.abs(spriteData[i][j] - 1);
									}else if(e.which == 3){ // right click
										e.preventDefault();
										spriteData2[i][j] = Math.abs(spriteData2[i][j] - 1);
									}
									$(this).addClass("color"+(spriteData[i][j] + 2*spriteData2[i][j]).toString(10));
								})
						);
					}
				}
				for(i=0;i<8;i++){
					for(j=0;j<8;j++){
						$("#container2").append(
							$("<div>")
								.addClass("pixel")
								.addClass("color"+(spriteData2[i][j]).toString(10))
								.data({
									i:i,
									j:j
								})
								.mousedown(function(e){
									i = $(this).data("i");
									j = $(this).data("j");
									$(this).removeClass (function (index, css) {
										return (css.match (/(^|\s)color\S+/g) || []).join(" ");
									});
									if(e.which == 1){ // left click
										e.preventDefault();
										
										spriteData2[i][j] = Math.abs(spriteData2[i][j] - 1);
									}
									$(this).addClass("color"+(spriteData2[i][j]).toString(10));
								})
						);
					}
				}
				
				
				$("#get").click(function(e){
					e.preventDefault();
					var buffer1Binary = "",
						buffer1Hex = "",
						buffer2Binary = "",
						buffer2Hex = "";
					for(i=0;i<8;i++){
						for(j=0;j<8;j++){
							if(spriteData[i][j] && !spriteData2[i][j]){
								buffer1Binary += "1";
							}else{
								buffer1Binary += "0";
							}
							if(spriteData2[i][j]){
								buffer2Binary += "1";
							}else{
								buffer2Binary += "0";
							}
						}
						buffer1Hex += pad(parseInt(buffer1Binary,2).toString(16),2);
						buffer2Hex += pad(parseInt(buffer2Binary,2).toString(16),2);
						buffer1Binary = "";
						buffer2Binary = "";
					}
					
					homepage.post("?get",
					{
						"buffer1":buffer1Hex,
						"buffer2":buffer2Hex
					},
					function(data){
						$("#data").val(data.data);
					});
				});
			});
		</script>
		<div id="container" oncontextmenu="return false;">
			
		</div><br>
		<input id="data"><br>
		<button id="get">Get Data</button><br>';
$html .= '<a href=".">&lt;&lt; Back</a>';
$sql->switchDb('soru_homepage');
echo $page->getPage('Masked Sprites',$html,$lang,$pathPartsParsed);
