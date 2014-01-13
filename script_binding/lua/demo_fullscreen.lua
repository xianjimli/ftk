function OnQuit(button)
	print("OnQuit: " .. button:GetText())
	Ftk.Quit()

	return RET_OK
end

function OnFullscreen(button)
	button:Toplevel():SetFullscreen(1)	
	return RET_OK
end

function OnUnfullscreen(button)
	button:Toplevel():SetFullscreen(0)	
	return RET_OK
end

function AppInit()
	win=FtkAppWindow.Create()
	win:SetText("Demo fullscreen")
	win:SetAttr(FTK_ATTR_QUIT_WHEN_CLOSE)
	
	width=win:Width()
	height=win:Height()

	button=FtkButton.Create(win, 0, 30, width/3, 50)
	button:SetText("全屏")
	button:SetClickedListener("OnFullscreen")

	button=FtkButton.Create(win, 2*width/3, 30, width/3, 50);
	button:SetText("正常")
	button:SetClickedListener("OnUnfullscreen")
	
	button=FtkButton.Create(win, width/3, height/2, width/3, 50);
	button:SetText("Quit")
	button:SetClickedListener("OnQuit")

	win:ShowAll(1)

	return 1
end

Ftk.Init(1, {"fullscreen"})
AppInit()
Ftk.Run()

