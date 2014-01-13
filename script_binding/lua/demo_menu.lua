function OnSave(item)
	print("OnSave: " .. item:GetText())
	return RET_OK
end

function OnQuit(item)
	print("OnQuit: " .. item:GetText())
	Ftk.Quit()

	return RET_OK
end

function OnPrepareOptionMenu(menu_panel)
	item = FtkMenuItem.Create(menu_panel)
	item:Show(1)
	item:SetText("Quit")
	item:SetClickedListener("OnQuit")
	
	item = FtkMenuItem.Create(menu_panel)
	item:Show(1)
	item:SetText("Save")
	item:SetClickedListener("OnSave")

	return RET_OK;
end

function AppInit()
	win=FtkAppWindow.Create()
	win:SetAttr(FTK_ATTR_QUIT_WHEN_CLOSE)
	win:SetText("Demo menu")

	width=win:Width()
	height=win:Height()
	
	label=FtkLabel.Create(win, 10, height/2, width-20, 30)
	label:SetText("press F2 to open menu")

	win:SetOnPrepareOptionsMenu("OnPrepareOptionMenu")
	win:ShowAll(1)

	return 1
end

Ftk.Init(1, {"menu"})
AppInit()
Ftk.Run()

