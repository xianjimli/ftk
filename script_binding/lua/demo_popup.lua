function OnMenuItemClicked(info)
	print("OnMenuItemClicked: " .. info.text)
end

items={"Cut", "Copy", "Paste", "Select All"}

function ShowPopup(type)
	local icon = FtkTheme.LoadImage(Ftk.DefaultTheme(), "info.png")
	local popup = FtkPopupMenu.Create(0, 0, 0, 200, icon, "Edit")
	info = FtkListItemInfo.Create();
	info.type = type;
	for i=1,#items do
		if i > 2 then
			info.state = 1
		else
			info.state = 0
		end
		info.text=items[i]
		popup:Add(info)
	end
	icon:Unref()
	popup:SetClickedListener("OnMenuItemClicked")
	popup:ShowAll(1)
end

function OnNormal(button)
	ShowPopup(FTK_LIST_ITEM_NORMAL)
	return RET_OK
end

function OnRadio(button)
	ShowPopup(FTK_LIST_ITEM_RADIO)
	return RET_OK
end

function OnCheckBox(button)
	ShowPopup(FTK_LIST_ITEM_CHECK)
	print("Clicked")
	return RET_OK
end

function OnQuit(button)
	Ftk.Quit()

	return RET_OK
end


function AppInit()
	win=FtkAppWindow.Create()
	win:SetText("Demo popup")
	win:SetAttr(FTK_ATTR_QUIT_WHEN_CLOSE)

	width=win:Width()
	height=win:Height()

	button=FtkButton.Create(win, 0, height/6, width/3, 50)
	button:SetText("Normal")
	button:SetClickedListener("OnNormal")

	button=FtkButton.Create(win, 2*width/3, height/6, width/3, 50)
	button:SetText("Radio")
	button:SetClickedListener("OnRadio")

	button=FtkButton.Create(win, 0, height/2, width/3, 50)
	button:SetText("CheckBox")
	button:SetClickedListener("OnCheckBox")

	button=FtkButton.Create(win, 2*width/3, height/2, width/3, 50)
	button:SetText("Quit")
	button:SetClickedListener("OnQuit")

	win:SetFocus(button)
	win:ShowAll(1)

	return 1
end

Ftk.Init(1, {"popup"})
AppInit()
Ftk.Run()

