function OnQuit(button)
	print("OnQuit: " .. button:GetText())
	Ftk.Quit()

	return RET_OK
end

function OnStart(button)
	wait_box=button:Toplevel():Lookup(1)
	wait_box:StartWaiting()

	return RET_OK
end

function OnStop(button)
	wait_box=button:Toplevel():Lookup(1)
	wait_box:StopWaiting()

	return RET_OK
end

function AppInit()
	win=FtkAppWindow.Create()
	win:SetAttr(FTK_ATTR_QUIT_WHEN_CLOSE)
	win:SetText("Demo waitbox")

	width=win:Width()
	height=win:Height()

	wait_box=FtkWaitBox.Create(win, width/2 - 16, height/4)
	wait_box:SetId(1)

	button=FtkButton.Create(win, 0, height/2, width/3, 50)
	button:SetText("Start")
	button:SetClickedListener("OnStart")

	button=FtkButton.Create(win, 2*width/3, height/2, width/3, 50)
	button:SetText("Stop")
	button:SetClickedListener("OnStop")
	
	button=FtkButton.Create(win, width/4, 3*height/4, width/2, 60)
	button:SetText("Quit")
	button:SetClickedListener("OnQuit")

	win:SetFocus(button)
	win:ShowAll(1)

	return 1
end

Ftk.Init(1, {"waitbox"})
AppInit()
Ftk.Run()

