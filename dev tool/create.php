<?php

if((!$security->isLoggedIn() || !($user_info['power']&128)) && $_GET['pwd']!='weetigaeVeezahth5Poh3Lah2uuFaephohx7oolaimiejaera5'){
	echo $page->getPage('Nope','<script type="text/javascript">getPageJSON("/");</script>Redirecting...',$lang,$pathPartsParsed);
	die();
}
$sql->switchDb('soru_gamebuino_rpg');
include_once(realpath(dirname(__FILE__)).'/functions.php');

function reverseEndian($s){
	return implode('',array_reverse(str_split($s,2)));
}
function dechexpad2($i,$num=2,$littleendian = false){
	$s = str_pad(dechex($i),$num,'0',STR_PAD_LEFT);
	if($littleendian){
		return reverseEndian($s);
	}
	return $s;
}

function rotate_buffer_left($in){
	$bin = hexstr2binstr($in);
	$newbin = '';
	for($i = 0;$i < 8;$i++){
		for($j = 7;$j >= 0;$j--){
			$newbin .= $bin[($j*8) + $i];
		}
	}
	return binstr2hexstr($newbin);
}

class Parser{
	private $bytes = 0;
	private $variables = [];
	private $offset = 0;
	private $out = '';
	private $functions = [];
	private $if_stack = [];
	private $while_stack = [];
	private $defines = [];
	private $extra_vars = ['player_x','player_y','map','world'];
	private $firstPass = true;
	private $labels = [];
	private $genericLabelCounter = 0;
	private $addressPrefix = '';
	private function getLabel(){
		return '++generic_label_'.($this->genericLabelCounter++).'++';
	}
	private function addLabel($label,$offset = 0){
		if(!$this->firstPass){
			return;
		}
		$this->labels[$this->addressPrefix.$label] = $this->bytes + $offset + $this->offset;
	}
	private function getAddress($label){
		if($this->firstPass){
			return '00000000';
		}
		return dechexpad2($this->labels[$this->addressPrefix.$label],8,true);
	}
	private function getVarNum($var){
		if(isset($this->variables[$var])){
			return dechexpad2($this->variables[$var]);
		}
		if(($i = array_search($var,$this->extra_vars)) !== false){
			return dechexpad2(0xFF - $i);
		}
	}
	private function getVar($var){
		if(($s = $this->getVarNum($var))){
			return '80'.$s;
		}
		$var = (int)$var;
		if($var < 0){
			$var += 256;
		}
		if($var == 0x80){
			return '8080';
		}
		return dechexpad2($var);
	}
	private function parseLine($line,$convertToBytes = true){
		$out = '';
		$line = trim($line);
		$matches = [];
		if(!preg_match('/^(\\w+)(\\((?:[^()]*(?:\\([^)]*\\))?)*\\))?;?$/',$line,$matches)){
			return '';
		}
		$function = $matches[1];
		$args = [];
		if(isset($matches[2])){
			preg_match_all('/(?:\\(|,)([^(),]*(?:\\([^)]+[^),]*\\))?)/',$matches[2],$matches,PREG_SET_ORDER);
			foreach($matches as $m){
				$m[1] = strtr($m[1],$this->defines);
				if(isset($this->functions[$function]) && isset($this->functions[$function]['unparsed_args']) && $this->functions[$function]['unparsed_args']){

				}else{
					if(preg_match('/^[\d\s*+\\/\\-()]+$/',$m[1])){
						$m[1] = eval("return $m[1];");
					}
				}
				$args[] = trim($m[1]);
			}
		}
		if(isset($this->functions[$function])){
			$fn = $this->functions[$function];
			$numArgs = sizeof($args);
			if($numArgs >= $fn['args_min'] && $numArgs <= $fn['args_max']){
				$s = $fn['fn']($args);
				if($convertToBytes){
					$s = hex2bin($s);
				}
				$out .= $s;
			}
		}
		return $out;
	}
	public function __construct(){
		$this->functions = [
			'fade_to_white' => [
				'args_min' => 0,
				'args_max' => 0,
				'fn' => function(){
					return '01';
				}
			],
			'fade_from_white' => [
				'args_min' => 0,
				'args_max' => 0,
				'fn' => function(){
					return '02';
				}
			],
			'set_map' => [
				'args_min' => 1,
				'args_max' => 1,
				'fn' => function($args){
					return '03'.$this->getVar($args[0]);
				}
			],
			'focus_cam' => [
				'args_min' => 0,
				'args_max' => 0,
				'fn' => function(){
					return '06';
				}
			],
			'update_screen' => [
				'args_min' => 0,
				'args_max' => 0,
				'fn' => function(){
					return '07';
				}
			],
			'set_var' => [
				'args_min' => 2,
				'args_max' => 2,
				'fn' => function($args){
					return '08'.$this->getVarNum($args[0]).$this->getVar($args[1]);
				}
			],
			'label' => [
				'args_min' => 1,
				'args_max' => 1,
				'fn' => function($args){
					$this->addLabel($args[0]);
				}
			],
			'goto' => [
				'args_min' => 1,
				'args_max' => 1,
				'fn' => function($args){
					return '09'.$this->getAddress($args);
				}
			],
			'jump_ifnot' => [
				'args_min' => 0,
				'args_max' => 0,
				'fn' => function($args){
					return '0a';
				}
			],
			'lt' => [
				'args_min' => 2,
				'args_max' => 2,
				'fn' => function($args){
					return '0b'.$this->getVar($args[0]).$this->getVar($args[1]);
				}
			],
			'gt' => [ // gt is like lt....with reversed arguments!
				'args_min' => 2,
				'args_max' => 2,
				'fn' => function($args){
					return '0b'.$this->getVar($args[1]).$this->getVar($args[0]);
				}
			],
			'add' => [
				'args_min' => 2,
				'args_max' => 2,
				'fn' => function($args){
					return '0c'.$this->getVarNum($args[0]).$this->getVar($args[1]);
				}
			],
			'inc' => [
				'args_min' => 1,
				'args_max' => 1,
				'fn' => function($args){
					return '0e'.$this->getVarNum($args[0]);
				}
			],
			'dec' => [
				'args_min' => 1,
				'args_max' => 1,
				'fn' => function($args){
					return '0f'.$this->getVarNum($args[0]);
				}
			],
			'call' => [
				'args_min' => 1,
				'args_max' => 1,
				'fn' => function($args){
					return '10'.$getAddress([$args[0]]);
				}
			],
			'ret' => [
				'args_min' => 0,
				'args_max' => 0,
				'fn' => function($args){
					return '11';
				}
			],
			'text' => [
				'args_min' => 1,
				'args_max' => 99999999999,
				'fn' => function($args){
					$s = implode(',',$args);
					$s = preg_replace_callback('/\\\\x([\da-fA-F]{2})/',function($matches){
						return hex2bin($matches[1]);
					},$s);
					$s = strtr($s,[
						'\\n' => "\xfe",
						'\\r' => "\xfd"
					]);
					$a = [];
					while(strlen($s)){
						if(strlen($s) > 19){
							for($i = 19;$i >= 0;$i--){
								if(isset($s[$i]) && $s[$i] == ' '){
									break;
								}
							}
							if($i <= 0){
								$i = 19;
							}
							$a[] = substr($s,0,$i);
							$s = ltrim(substr($s,$i));
						}else{
							$a[] = $s;
							break;
						}
					}
					$s = '';
					$alt = true;
					foreach($a as $e){
						$s .= $e;
						if($alt = !$alt){
							// pagebreak
							$s .="\xfd";
						}else{
							// linebreak
							$s .= "\xfe";
						}
					}
					$s = substr($s,0,-1);
					return '04'.bin2hex($s).'ff';
				}
			],
			'add_enemy' => [
				'args_min' => 3,
				'args_max' => 3,
				'fn' => function($args){
					return '05'.$this->getVar($args[0]).$this->getVar($args[1]).$this->getVar($args[2]);
				}
			],

			'hex' => [
				'args_min' => 1,
				'args_max' => 1,
				'unparsed_args' => true,
				'fn' => function($args){
					return $args[0];
				}
			],
			'put_address' => [
				'args_min' => 1,
				'args_max' => 1,
				'fn' => function($args){
					return $this->getAddress($args[0]);
				}
			],
			'set_address_prefix' => [
				'args_min' => 0,
				'args_max' => 1,
				'fn' => function($args){
					if(sizeof($args) == 0){
						$this->addressPrefix = '';
					}else{
						$this->addressPrefix = $args[0];
					}
				}
			],
			
			'return_false' => [
				'args_min' => 0,
				'args_max' => 0,
				'fn' => function($args){
					return 'fe';
				}
			],
			'return_true' => [
				'args_min' => 0,
				'args_max' => 0,
				'fn' => function($args){
					return 'ff';
				}
			],
			
			'define_vars' => [
				'args_min' => 1,
				'args_max' => 10,
				'fn' => function($args){
					foreach($args as $i => $a){
						$this->variables[$a] = $i;
					}
				}
			],
			'clear_vars' => [
				'args_min' => 0,
				'args_max' => 0,
				'fn' => function(){
					$this->variables = [];
				}
			],
			'set_player_pos' => [
				'args_min' => 2,
				'args_max' => 5,
				'fn' => function($args){
					$out = '';
					$size = sizeof($args);
					$update = false;
					if($size > 2 && $args[$size-1] == 'update'){
						$update = true;
						$size--;
					}
					$focus = false;
					if($size > 2 && $args[$size-1] == 'focus'){
						$focus = true;
						$size--;
					}
					$i = 0;
					if($size > 2){
						$out .= $this->functions['set_map']['fn']([$args[$i++]]);
					}
					$out .= $this->functions['set_var']['fn'](['player_x',$args[$i++]]);
					
					$out .= $this->functions['set_var']['fn'](['player_y',$args[$i++]]);
					if($focus){
						$out .= '06';
					}
					if($update){
						$out .= '07';
					}
					return $out;
				}
			],
			'return' => [
				'args_min' => 1,
				'args_max' => 1,
				'fn' => function($args){
					if($args[0] == 'true'){
						return 'ff';
					}
					return 'fe';
				}
			],

			'if' => [
				'args_min' => 1,
				'args_max' => 1,
				'fn' => function($args){
					$out = '0a'.$this->parseLine($args[0],false);
					$label = $this->getLabel();
					$this->if_stack[] = $label;
					return $out.$this->getAddress($label);
				}
			],
			'else' => [
				'args_min' => 0,
				'args_max' => 0,
				'fn' => function(){
					$label = array_pop($this->if_stack);
					$this->addLabel($label,5);
					
					$out = '09';
					$label = $this->getLabel();
					$this->if_stack[] = $label;
					return $out.$this->getAddress($label);
				}
			],
			'endif' => [
				'args_min' => 0,
				'args_max' => 0,
				'fn' => function(){
					$label = array_pop($this->if_stack);
					$this->addLabel($label);
				}
			],

			'do' => [
				'args_min' => 0,
				'args_max' => 0,
				'fn' => function(){
					$label = $this->getLabel();
					$this->addLabel($label);
					$this->while_stack[] = $label;
				}
			],
			'while' => [
				'args_min' => 1,
				'args_max' => 1,
				'fn' => function($args){
					$out = '0d'.$this->parseLine($args[0],false);
					$label = array_pop($this->while_stack);
					$out .= $this->getAddress($label);
					return $out;
				}
			]
		];
	}
	public function parse($script,$offset = 0,$def = []){
		$this->defines = $def;
		$this->offset = $offset;
		$this->variables = [];
		$this->bytes = 0;
		$out = '';
		$this->if_stack = [];
		$this->while_stack = [];
		$this->labels = [];
		$this->firstPass = true;
		$this->genericLabelCounter = 0;
		$this->addressPrefix = '';
		foreach(explode("\n",strtolower($script)) as $line){
			$out .= $this->parseLine($line);
			$this->bytes = strlen($out);
		}
		$this->addressPrefix = '';
		$this->bytes = 0;
		$out = '';
		$this->firstPass = false;
		$this->genericLabelCounter = 0;
		foreach(explode("\n",strtolower($script)) as $line){
			$out .= $this->parseLine($line);
			$this->bytes = strlen($out);
		}

		global $defines;
		if(sizeof($this->variables) > $defines['script_num_vars']){
			$defines['script_num_vars'] = sizeof($this->variables);
		}
		return $out;
	}
}
$parser = new Parser();

$sql->query("UPDATE `sprites` SET `in_use`=0 WHERE 1");
$tilemaps = $sql->query("SELECT `data` FROM `tilemaps`",[]);
$spritesInUse = [];
foreach($tilemaps as $t){
	$td = json_decode($t['data'],true);
	foreach($td as $s){
		if(!in_array($s,$spritesInUse)){
			$spritesInUse[] = $s;
		}
	}
}
foreach($spritesInUse as $s){
	$sql->query("UPDATE `sprites` SET `in_use`=1,`asm_id`=0 WHERE `id`=%d",[$s]);
}



$spritesWithId = [];
$spritesLUT = [];
$idCounter = 0;
$defines = [];
$defines['script_num_vars'] = 0; // else the parser will error
$sprites_inanimate = $sql->query("SELECT `id` FROM `sprites` WHERE `animated`=0");
foreach($sprites_inanimate as $s){
	$s = $s['id'];
	if($s!==NULL && in_array($s,$spritesInUse)){
		$asmId = dechexpad2($idCounter,4);
		$sql->query("UPDATE `sprites` SET `asm_id`='%s' WHERE `id`=%d",[$asmId,$s]);
		$spritesLUT[$s] = $asmId;
		$idCounter++;
		$spritesWithId[] = $s;
	}
}
$defines['sprites_first_animated'] = $idCounter;


foreach($spritesInUse as $s){
	if(!in_array($s,$spritesWithId)){
		$asmId = dechexpad2($idCounter,4);
		$sql->query("UPDATE `sprites` SET `asm_id`='%s' WHERE `id`=%d",[$asmId,$s]);
		$spritesLUT[$s] = $asmId;
		$idCounter++;
		$spritesWithId[] = $s;
	}
}





$mapDimensions = $sql->query("SELECT MIN(`x`) AS `minx`,MAX(`x`)-MIN(`x`)+1 AS `width`,MIN(`y`) AS `miny` FROM `tilemaps`",[],0);

$html = '<h1>Sprites</h1><textarea style="width:100%;height:500px;">';


$file = "sprites:\n";
$spritesdat = '';
foreach($spritesWithId as $sid){
	$s = $sql->query("SELECT `buffer1`,`buffer2`,`name`,`asm_id`,`animated`,`walkable` FROM `sprites` WHERE `id`=%d",[$sid],0);
	if($s['name']){
		$defines['sprite_'.$s['name']] = '0x'.$s['asm_id'];
	}
	$out = "\t.db ";
	$s['walkable'] = json_decode($s['walkable'],true);
	if(!$s['walkable']){
		$out .= "0x00\n";
		$spritesdat .= hex2bin("00");
	}else{
		$bin = '0000';
		for($i = 0;$i < 4;$i++){
			$bin[$i] = $s['walkable']['map'][$i]?'1':'0';
		}
		$hex = dechex($s['walkable']['type']).getHexDiget($bin);
		$out .= "0x$hex\n";
		$spritesdat .= hex2bin($hex);
	}
	$out .= "\t.db ";
	$s['buffer1'] = rotate_buffer_left($s['buffer1']);
	for($i = 0;$i < 16;$i +=2){
		$out .= "0x".substr($s['buffer1'],$i,2).",";
		$spritesdat .= hex2bin(substr($s['buffer1'],$i,2));
	}
	if($s['animated']){
		$out .= ' ';
		$s['buffer2'] = rotate_buffer_left($s['buffer2']);
		for($i = 0;$i < 16;$i +=2){
			$out .= "0x".substr($s['buffer2'],$i,2).",";
			$spritesdat .= hex2bin(substr($s['buffer2'],$i,2));
		}
	}
	$file .= substr($out,0,-1)."\n";
}
foreach($spritesLUT as $normalId => $asmId){
	$defines['sprite_'.$normalId] = "0x".$asmId;
}


$html .= $file.'</textarea><h1>Tilemaps</h1><textarea style="width:100%;height:500px;">';




$defines['datfile_start_sprites'] = 0;
$defines['datfile_start_tilemaplut'] = strlen($spritesdat);
$datfile = $spritesdat;


$oldMapId = -1;
$tilemaps = '';
$tilemapslut = '';
$tilemapslutlut = '';
$asmMapId = -1; // will be increased to zero
$genericAddressPrefix = 0;
foreach($sql->query("SELECT `data`,`id`,`x`,`y`,`area`,`mapId`,`exitScript`,`entryScript` FROM `tilemaps` ORDER BY `mapId` ASC",[]) as $t){
	if($t['mapId'] != $oldMapId){
		$asmMapId++;
		$tilemapslutlut .= "put_address(++world#$asmMapId++)\n";
		$tilemapslut .= "label(++world#$asmMapId++)\n";
		$oldMapId = $t['mapId'];
		$defines['world_'.$t['mapId']] = $asmMapId;
		$defines['world_'.strtolower($sql->query("SELECT `name` FROM `maps` WHERE `id`=%d",[$t['mapId']],0)['name'])] = $asmMapId;
	}
	$tilemaps .= "label(++tilemap#$t[id]++)\n";

	$td = json_decode($t['data'],true);
	$k = 0;
	$xPos = (int)$t['x'] - (int)$mapDimensions['minx'];
	$yPos = (int)$t['y'] - (int)$mapDimensions['miny'];
	$mapId = $yPos * (int)$mapDimensions['width'] + $xPos;
	$sql->query("UPDATE `tilemaps` SET `asm_id`='%s' WHERE `id`=%d",[dechexpad2($mapId),$t['id']]);
	
	$tilemapslut .= "hex(".dechexpad2($mapId).")\nput_address(++tilemap#$t[id]++)\n";
	$defines['tilemap_'.$t['id']] = $mapId;

	$header = 0;
	if(trim($t['exitScript'])){
		$header |= 1;
	}
	if(trim($t['entryScript'])){
		$header |= 2;
	}

	$s = dechexpad2($header);
	
	$hexData = ['0x00'];
	for($i = 0;$i < 8;$i++){
		for($j = 0;$j < 12;$j++){
			$s .= reverseEndian($spritesLUT[$td[$k]]);
			$k++;
		}
	}

	$tilemaps .= "hex($s)\n";
	if(trim($t['entryScript'])){
		$tilemaps .= "put_address(++tilemaps#$t[id]#entryscript++)\n";
	}elseif(trim($t['exitScript'])){
		$tilemaps .= "hex(00000000)\n";
	}
	if(trim($t['exitScript'])){
		$tilemaps .= "put_address(++tilemaps#$t[id]#exitscript++)\n";
	}
	if(trim($t['entryScript'])){
		$genericAddressPrefix++;
		$tilemaps .= "label(++tilemaps#$t[id]#entryscript++)\nset_address_prefix(##generic$genericAddressPrefix##)\n$t[entryScript]\nclear_vars\nset_address_prefix()\n";
	}
	if(trim($t['exitScript'])){
		$genericAddressPrefix++;
		$tilemaps .= "label(++tilemaps#$t[id]#exitscript++)\nset_address_prefix(##generic$genericAddressPrefix##)\n$t[exitScript]\nclear_vars\nset_address_prefix()\n";
	}

}
$scriptOffset = strlen($datfile);
$file = $tilemapslutlut.$tilemapslut.$tilemaps;



$defines['datfile_start_tilemap'] = $scriptOffset;
$datfile .= $parser->parse($file,$scriptOffset,$defines);;

$defines['datfile_tilemaps_header_size'] = '1';
$defines['datfile_tilemap_size'] = 1 + (2*8*12);
$defines['datfile_tilemaps_width'] = $mapDimensions['width'];



$defaultMap = $sql->query("SELECT `asm_id` FROM `tilemaps` WHERE `id`=%d",[$vars->get('defaultMap')],0);
$defines['tilemaps_defaultmap'] = "0x".$defaultMap['asm_id'];
$maxspritespace = $sql->query("SELECT MAX(`spritespace`) AS `spritespace` FROM `tilemaps`",[],0);
$defines['tilemaps_spritespace'] = $maxspritespace['spritespace'];





$html .= $file.'</textarea><h1>Defines</h1><textarea style="width:100%;height:500px;">';


$defines['datfile_start_script'] = strlen($datfile);

$scriptOffset = ($sql->query("SELECT COUNT(`id`) AS num FROM `eventTiles`",[],0)['num'] * 6) + 1 + $defines['datfile_start_script'];

$scriptlutdat = '';
$scriptdat = '';
$scripts = $sql->query("SELECT `code`,`refId`,`x`+(`y`)*12 AS `offset` FROM `eventTiles`");
$defines['script_num_vars'] = 1; // else c++ will try to create a zero-length array
foreach($scripts as $s){
	if($s['code']!==NULL){
		$sc = $parser->parse($s['code'],$scriptOffset,$defines);
		$scriptlutdat .= hex2bin(dechexpad2((int)$s['offset'])).hex2bin(dechexpad2($defines['tilemap_'.$s['refId']])).hex2bin(dechexpad2($scriptOffset,8,true));
		$scriptOffset += strlen($sc);
		$scriptdat .= $sc;
	}
}
$scriptlutdat .= hex2bin('ff');

$datfile .= $scriptlutdat;
$datfile .= $scriptdat;
file_put_contents(realpath(dirname(__FILE__)).'/DATA.DAT',$datfile);
$file = "";
foreach($defines as $label => $value){
	$file .= '#define '.strtoupper($label)." $value\n";
}
file_put_contents(realpath(dirname(__FILE__)).'/defines.h',$file);

$html .= $file.'</textarea><hr><a href="/reuben3">&lt;&lt; Back</a>';


$sql->switchDb('soru_homepage');
echo $page->getPage('Create Asm',$html,$lang,$pathPartsParsed);
?>
