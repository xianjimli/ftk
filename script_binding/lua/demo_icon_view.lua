function OnQuit(button)
	Ftk.Quit()

	return RET_OK
end

function OnMore(button)
	local icon_view = button:Toplevel():Lookup(100);
	local item=FtkIconViewItem.Create()
	item.icon = FtkTheme.LoadImage(Ftk.DefaultTheme(), "flag-32.png")

	for i=1, 10 do
		item.text = "item" .. tostring(i)
		icon_view:Add(item)
	end

	item.icon:Unref()
	item=nil

	return RET_OK
end

function OnItemClicked(item)
	print("OnItemClicked: " .. item.text)
	return RET_OK
end

function AppInit()
	win=FtkAppWindow.Create()
	win:SetText("Demo icon view")
	win:SetAttr(FTK_ATTR_QUIT_WHEN_CLOSE)

	width=win:Width()
	height=win:Height()

	button=FtkButton.Create(win, 10, 0, width/3-10, 60)
	button:SetText("More")
	button:SetClickedListener("OnMore")

	button=FtkButton.Create(win, 2*width/3, 0, width/3-10, 60)
	button:SetText("Quit")
	button:SetClickedListener("OnQuit")

	item=FtkIconViewItem.Create()
	item.icon = FtkTheme.LoadImage(Ftk.DefaultTheme(), "flag-32.png")
	
	icon_view = FtkIconView.Create(win, 5, 70, width-10, height-80)
	icon_view:SetId(100);
	icon_view:SetClickedListener("OnItemClicked")
	for i=1, 10 do
		item.text = "item" .. tostring(i)
		icon_view:Add(item)
	end
	item.icon:Unref()
	item=nil

	win:SetFocus(button)
	win:ShowAll(1)

	return 1
end

Ftk.Init(1, {"iconview"})
AppInit()
Ftk.Run()

