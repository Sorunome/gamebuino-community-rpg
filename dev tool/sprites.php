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
if(isset($_GET['edit'])){
	$html = '<style type="text/css">
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
			</style>
			<script type="text/javascript">
				function pad(n, width, z){
					z = z || "0";
					n = n + "";
					return n.length >= width ? n : new Array(width - n.length + 1).join(z) + n;
				}
				$(document).ready(function(){
					var spriteData = [';
	
						$r = $sql->query("SELECT `buffer1`,`buffer2`,`animated`,`walkable` FROM `sprites` WHERE `id`=%d",[(int)$_GET['edit']],0);
						$buffer1bin = hexstr2binstr($r['buffer1']);
						$buffer2bin = hexstr2binstr($r['buffer2']);
						$s = '';
						$k = 0;
						for($i = 0;$i < 8;$i++){
							$s .= '[';
							for($j = 0;$j < 8;$j++){
								$c = $buffer1bin[$k];
								$s .= $c.',';
								$k++;
							}
							$s = substr($s,0,-1).'],';
						}
						$html .= substr($s,0,-1);
					$html .= '],
						spriteData2 = [';
						$s = '';
						$k = 0;
						for($i = 0;$i < 8;$i++){
							$s .= '[';
							for($j = 0;$j < 8;$j++){
								$c = $buffer2bin[$k];
								$s .= $c.',';
								$k++;
							}
							$s = substr($s,0,-1).'],';
						}
						$html .= substr($s,0,-1);
					$html .= '],
						animated='.($r['animated']?'true':'false').',
						walkable=';
					$r['walkable'] = json_decode($r['walkable'],true);
					if(!$r['walkable']){
						$r['walkable'] = [
							'type' => 0,
							'map' => [false,false,false,false]
						];
					}
					$html .=json_encode($r['walkable']).',
						i,j;
					for(i=0;i<8;i++){
						for(j=0;j<8;j++){
							$("#container").append(
								$("<div>")
									.addClass("pixel")
									.addClass("color"+(spriteData[i][j]).toString(10))
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
											
											spriteData[i][j] = Math.abs(spriteData[i][j] - 1);
										}
										$(this).addClass("color"+(spriteData[i][j]).toString(10));
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
					if(animated){
						$("#do_animate")[0].checked = true;
					}else{
						$("#container2").hide();
					}
					$("#do_animate").change(function(){
						animated = this.checked;
						if(animated){
							$("#container2").show();
						}else{
							$("#container2").hide();
						}
					});
					
					$("#walkable_type").val(walkable.type).change(function(){
						walkable.type = this.value;
					});
					$.each(walkable.map,function(i,v){
						$("#walkable_matrix_"+i).change(function(){
							walkable.map[i] = this.checked;
						})[0].checked = v;
					});
					
					$("#save").click(function(e){
						e.preventDefault();
						var buffer1Binary = "",
							buffer1Hex = "",
							buffer2Binary = "",
							buffer2Hex = "";
						for(i=0;i<8;i++){
							for(j=0;j<8;j++){
								if(spriteData[i][j]){
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
						
						homepage.post("?save=';
							$html .= $_GET['edit'];
							$html .= '",
						{
							"buffer1":buffer1Hex,
							"buffer2":buffer2Hex,
							"animated":animated?1:0,
							"walkable":JSON.stringify(walkable)
						},
						function(data){
							alert(data);
						});
					});
					$("#delete").click(function(e){
						e.preventDefault();
						if(confirm("are you sure you want to delete this sprite?")){
							homepage.get("sprites?delete='.(int)$_GET['edit'].'",function(data){
								getPageJSON("sprites");
							});
						}
					});
				});
			</script>
			<h1>Sprite ID: '.(int)$_GET['edit'].'</h1>
			<div id="container" oncontextmenu="return false;">
				
			</div>
			<div id="container2" oncontextmenu="return false;">
				
			</div><br>
			<label>Animate: <input type="checkbox" id="do_animate"></label><br>
			Walkable type: <select id="walkable_type">
				<option value="0">Wall</option>
				<option value="1">Water</option>
				<option value="2">Cliff (fall)</option>
				<option value="3">Script</option>
				<option value="4">Manual script</option>
			</select><br>Matrix: <table><tr><td><input type="checkbox" id="walkable_matrix_0"></td><td><input type="checkbox" id="walkable_matrix_1"></td></tr><tr><td><input type="checkbox" id="walkable_matrix_2"></td><td><input type="checkbox" id="walkable_matrix_3"></td></tr></table>
			<button id="save">Save</button><button id="delete" style="float:right;">Delete</button><br>
			<a href="sprites">&lt;&lt; Back</a>';
	$sql->switchDb('soru_homepage');
	echo $page->getPage('Edit Sprite',$html,$lang,$pathPartsParsed);
}elseif(isset($_GET['save'])){
	$sql->query("UPDATE `sprites` SET `buffer1`='%s',`buffer2`='%s',`animated`=%d,`walkable`='%s' WHERE `id`=%d",[$_POST['buffer1'],$_POST['buffer2'],$_POST['animated'],$_POST['walkable'],(int)$_GET['save']]);
	echo 'Saved';
}elseif(isset($_GET['delete'])){
	$sql->query("DELETE FROM `sprites` WHERE `id`=%d",[(int)$_GET['delete']]);
}elseif(isset($_GET['new'])){
	$sql->query("INSERT INTO `sprites` (`buffer1`,`buffer2`) VALUES ('0000000000000000','0000000000000000')");
	$sql->switchDb('soru_homepage');
	echo $page->getPage('Nope','<script type="text/javascript">getPageJSON("sprites");</script>Redirecting...',$lang,$pathPartsParsed);
}elseif(isset($_GET['info'])){
	header('Content-Type: text/json');
	$i = $sql->query("SELECT `id`,`name`,`animated` FROM `sprites` WHERE `id`=%d",[(int)$_GET['info']],0);
	$info = [];
	$info['id'] = (int)$i['id'];
	if($i['name']){
		$info['name'] = $i['name'];
	}
	$info['animated'] = $i['animated']?'yes':'no';
	echo json_encode($info);
}else{
	$html = '<style type="text/css">
		#spritesCont > a {
			margin:0;
			padding:0;
		}
		#spritesCont > a > img {
			border:1px solid red;
			margin:0;
			padding:0;
		}
		</style>
		<div id="spritesCont">';
	
	$sprites = $sql->query("SELECT `id` FROM `sprites` WHERE 1");
	foreach($sprites as $s){
		$html .= '<a href="?edit='.$s['id'].'"><img src="disp?sprite='.$s['id'].'"></a>';
	}
	$html .='</div><hr>
	<a href="?new">New</a><br>
	<a href=".">&lt;&lt; Back</a>';
	$sql->switchDb('soru_homepage');
	echo $page->getPage('Sprites',$html,$lang,$pathPartsParsed);
}
?>
