function OnQuit(button)
	Ftk.Quit()

	return RET_OK
end

function AppInit()
	win=FtkAppWindow.Create()
	win:SetText("Demo entry")
	win:SetAttr(FTK_ATTR_QUIT_WHEN_CLOSE)
	
	width=win:Width()
	height=win:Height()

	entry=FtkEntry.Create(win, 10, 30, width - 20, 30);
	entry:SetText("Single line editor")

	entry=FtkEntry.Create(win, 10, 80, width - 20, 30)
	entry:SetText("Single line editor, that means you can input a one line only.")

	entry=FtkEntry.Create(win, 10, 130, width - 20, 30);
	entry:SetText("Single line editor, 也就是说你只能输入一行文字.")

	button=FtkButton.Create(win, width/4, height/2, width/2, 60)
	button:SetText("Quit")
	button:SetClickedListener("OnQuit")
	
	entry=FtkEntry.Create(win, 10, height-60, width - 20, 30)
	entry:SetText("Single line editor, that means you can input a one line only.")

	win:SetFocus(button)
	win:ShowAll(1)

	return 1
end

Ftk.Init(1, {"entry"})
AppInit()
Ftk.Run()

