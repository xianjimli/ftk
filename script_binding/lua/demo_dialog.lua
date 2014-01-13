function OnQuit(button)
	Ftk.Quit()
	return RET_OK
end

function OnNormalQuit(button)
	print("OnNormalQuit")
	button:Toplevel():Unref();
	return RET_OK;
end

function OnModalQuit(button)
	print("OnModalQuit")
	return RET_QUIT;
end

function CreateDialog(modal)
	dialog=FtkDialog.Create(0, 40, 320, 240)
	dialog:SetAnimationHint("dialog")
	width=dialog:Width()
	height=dialog:Height()

	theme=Ftk.DefaultTheme()
	icon=theme:LoadImage("info.png")
	dialog:SetIcon(icon)

	label=FtkLabel.Create(dialog, width/6, height/4, 5*width/6, 20)
	label:SetText("Are you sure to quit?")
	
	button=FtkButton.Create(dialog, width/6, height/2, width/3, 50)
	button:SetText("yes")
	if modal==1 then
		button:SetClickedListener("OnModalQuit")
	else
		button:SetClickedListener("OnNormalQuit")
	end
	button=FtkButton.Create(dialog, width/2, height/2, width/3, 50)
	button:SetText("no")
	if modal==1 then
		button:SetClickedListener("OnModalQuit")
	else
		button:SetClickedListener("OnNormalQuit")
	end

	dialog:ShowAll(1)
	if modal==1 then
		dialog:Run()
		dialog:Unref()
		print("DialogQuit");
	end
end

function OnNormal(button)
	CreateDialog(0)
	return RET_OK
end

function OnModal(button)
	CreateDialog(1)
	return RET_OK
end

function AppInit()
	win=FtkAppWindow.Create()
	win:SetText("Demo dialog")
	win:SetAttr(FTK_ATTR_QUIT_WHEN_CLOSE)

	local width=win:Width()
	local height=win:Height()

	button=FtkButton.Create(win, 0, height/6, width/3, 50)
	button:SetText("Norma")
	button:SetClickedListener("OnNormal")

	button=FtkButton.Create(win, 2*width/3, height/6, width/3, 50)
	button:SetText("Model")
	button:SetClickedListener("OnModal")

	button=FtkButton.Create(win, width/4, height/2, width/2, 60)
	button:SetText("quit")
	button:SetClickedListener("OnQuit")
	
	win:SetFocus(button)
	win:ShowAll(1)

	return 1
end

Ftk.Init(1, {"dialog"})
AppInit()
Ftk.Run()

