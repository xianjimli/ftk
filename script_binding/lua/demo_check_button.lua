function OnQuit(button)
	Ftk.Quit()

	return RET_OK
end

function OnClicked(button)
	print("OnClicked")
	return RET_OK
end

function AppInit()
	win=FtkAppWindow.Create()
	win:SetAttr(FTK_ATTR_QUIT_WHEN_CLOSE)
	win:SetText("Demo check button")

	width=win:Width()
	height=win:Height()

	width = width/2 - 10;
	button=FtkCheckButton.Create(win, 0, 10, width, 50)
	button:SetText("show")
	button:SetClickedListener("OnClicked")

	button=FtkCheckButton.Create(win, width + 10, 10, width, 50)
	button:SetText("hide")
	button:SetClickedListener("OnClicked")

	group=FtkGroupBox.Create(win, 0, 60, 2 * width, 60)
	button=FtkCheckButton.CreateRadio(group, 0, 10, width, 50);
	button:SetText("Male")
	button:SetClickedListener("OnClicked")

	button=FtkCheckButton.CreateRadio(group, width + 10, 10, width, 50)
	button:SetText("Female")
	button:SetClickedListener("OnClicked")

	group=FtkGroupBox.Create(win, 0, 120, 2 * width, 60)
	button=FtkCheckButton.CreateRadio(group,0, 10, width, 50)
	button:SetText("1(图标右)")
	button:SetIconPosition(1)
	button:SetClickedListener("OnClicked")

	button=FtkCheckButton.CreateRadio(group, width + 10, 10, width, 50)
	button:SetText("2(图标右)")
	button:SetIconPosition(1)
	button:SetClickedListener("OnClicked")

	button=FtkButton.Create(win, width/2, 3*height/4, width, 60)
	button:SetText("Quit")
	button:SetClickedListener("OnQuit")

	win:SetText("check button demo")
	win:SetFocus(button)
	win:ShowAll(1)

	return 1
end

Ftk.Init(1, {"checkbutton"})
AppInit()
Ftk.Run()

