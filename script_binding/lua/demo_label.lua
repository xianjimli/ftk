times=4
app_win=nil

function OnTimer()
	label = app_win:Lookup(100)	
	label:SetText("Quit after " .. times .. " seconds.")
	times=times-1

	if times > 0 then 
		return RET_OK 
	else
		Ftk.Quit()		
		return RET_REMOVE
	end
end

function AppInit()
	win=FtkAppWindow.Create()
	app_win=win
	win:SetText("Demo Label")
	win:SetAttr(FTK_ATTR_QUIT_WHEN_CLOSE)

	width=win:Width()
	height=win:Height()

	label=FtkLabel.Create(win, 10, 10, width - 20, 20)
	label:SetText("中文文本");

	label=FtkLabel.Create(win, 10, 40, width - 20, 20)
	label:SetText("English Text");

	label=FtkLabel.Create(win, 50, height/2-30, width, 20)
	label:SetText("Quit after 5 seconds.")
	label:SetId(100)

	label=FtkLabel.Create(win, 5, height/2, width-10, height/2-20)

	gc=FtkGc.Create()
	gc.mask=FTK_GC_BG
	gc.bg.a = 0xff;
	gc.bg.r = 0xF0;
	gc.bg.g = 0xF0;
	gc.bg.b = 0x80;
	label:SetGc(FTK_WIDGET_INSENSITIVE, gc)
	label:UnsetAttr(FTK_ATTR_TRANSPARENT)

	label:SetText("代码风格的一致性对代码可读性的帮助很大。FTK的文件名、函数名、变量名和类型名都遵循统一的命名风格")

	timer=FtkSourceTimer.Create(1000, "OnTimer")
	Ftk.DefaultMainLoop():AddSource(timer)

	win:ShowAll(1)

	return RET_OK;
end

Ftk.Init(1, {"label"})
AppInit()
Ftk.Run()


