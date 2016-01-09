var fs = require("fs");

var myArgs = process.argv.slice(2);
var node = process.argv[0];
var cmd = process.argv[1];

if(myArgs.length < 2) {
	console.log("usage: "+node + " "+cmd+" inputfile ouputdir\n");
	process.exit(0);
}
else {
	inputFile = myArgs[0];
	outputDir = myArgs[1];

	console.log("input file:" + inputFile);
	console.log("output dir:" + outputDir);
	console.log("----------------------------------------------------");
}

var UIElement = {};
UIElement.X_FIX_LEFT = 0;
UIElement.X_FIX_RIGHT = 1;
UIElement.X_SCALE = 2;
UIElement.X_CENTER_IN_PARENT = 3;
UIElement.X_LEFT_IN_PARENT   = 4;
UIElement.X_RIGHT_IN_PARENT  = 5;

UIElement.Y_FIX_TOP = 0;
UIElement.Y_FIX_BOTTOM = 1;
UIElement.Y_SCALE = 2;
UIElement.Y_MIDDLE_IN_PARENT = 3;
UIElement.Y_TOP_IN_PARENT    = 4;
UIElement.Y_BOTTOM_IN_PARENT = 5;

UIElement.HEIGHT_FIX = 0;
UIElement.HEIGHT_SCALE = 1;
UIElement.HEIGHT_FILL_PARENT = 2;

UIElement.WIDTH_FIX = 0;
UIElement.WIDTH_SCALE = 1;
UIElement.WIDTH_FILL_PARENT = 2;

UIElement.X_ATTRS = ["fix_left", "fix_right", "scale", "center_in_parent", "left_in_parent", "right_in_parent"];
UIElement.Y_ATTRS = ["fix_top", "fix_bottom", "scale", "middle_in_parent", "top_in_parent", "bottom_in_parent"];
UIElement.W_ATTRS = ["fix", "scale", "fill_parent"];
UIElement.H_ATTRS = ["fix", "scale", "fill_parent"];

var str = fs.readFileSync(inputFile);
var jso = JSON.parse(str);

var typeMapTable = {
	"ui-label":"label",
	"ui-edit":"entry",
	"ui-button":"button"
};

function mapType(type) {
	return typeMapTable[type];
}

function generateWidget(widget) {
	var type = mapType(widget.type);

	if(!type) {
		console.log("- skip not supported " + widget.type);
		return "";
	}

	var x = Math.round(widget.x);
	var y = Math.round(widget.y);
	var w = Math.round(widget.w);
	var h = Math.round(widget.h);
	var xParam = widget.xParam;
	var yParam = widget.yParam;
	var wParam = widget.widthParam;
	var hParam = widget.heightParam;

	var xAttr = widget.xAttr;
	var yAttr = widget.yAttr;
	var wAttr = widget.widthAttr;
	var hAttr = widget.heightAttr;

	var value = widget.text;
	var xattrs = 'x="'+x + '" xattr="'+xAttr+'" xparam="'+xParam+'" ';
	var yattrs = 'y="'+y + '" yattr="'+yAttr+'" yparam="'+yParam+'" ';
	var wattrs = 'w="'+w + '" wattr="'+wAttr+'" wparam="'+wParam+'" ';
	var hattrs = 'h="'+h + '" hattr="'+hAttr+'" hparam="'+hParam+'" ';

	var str = '\t<'+type+' id="'+widget.name + '" ' + xattrs + yattrs + wattrs + hattrs + 'value="'+value+'" />\n';

	return str;
}

function generateWin(win) {
	var name = win.name.replace(/-/, "_");
	
	var str = '"<?xml version="1.0" encoding="utf-8"?>"\n';
	str += '<window  value="'+win.name+'" visible="1">\n';
	
	for(var i = 0; i < win.children.length; i++) {
		var iter = win.children[i];
		str += generateWidget(iter);		
	}
	str += '</window>\n';

	var fileName = "output/" + name + ".xml";

	console.log("* write "+name+" to file: " + fileName);
	fs.writeFileSync(fileName, str);

}

function generateAll(wm) {
	for(var i = 0; i < wm.children.length; i++) {
		var win = wm.children[i];
		generateWin(win);
	}
}

if(!fs.existsSync(outputDir)) {
	fs.mkdirSync(outputDir);
}

generateAll(jso.wm);
