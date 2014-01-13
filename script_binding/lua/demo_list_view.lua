model=nil

function OnQuit(button)
	Ftk.Quit()

	return RET_OK
end

function OnMore(button)
	info=FtkListItemInfo.Create()
	for i =1,6 do
		info.text="list item " .. i
		model:Add(info)
	end
	info=nil

	return RET_OK
end

function OnListClicked(list)
	model=list:GetModel()
	i=list:GetSelected()

	print("i=" .. i)
	print("total=".. model:GetTotal())

	return RET_OK
end

function AppInit()
	win=FtkAppWindow.Create()
	win:SetAttr(FTK_ATTR_QUIT_WHEN_CLOSE)
	win:SetText("Demo icon view")

	width=win:Width()
	height=win:Height()

	list=FtkListView.Create(win, 10, 5, width - 20, 3 * height/4-5)
	list:SetClickedListener("OnListClicked")

	model=FtkListModelDefault.Create(10)
	render=FtkListRenderDefault.Create()
	
	info=FtkListItemInfo.Create()
	for i =1,10 do
		info.text="list item " .. i
		model:Add(info)
	end
	info=nil

	list:Init(model, render, 40)
	model:Unref()

	button=FtkButton.Create(win, 10, 3 * height/4 + 20, width/3-10, 60)
	button:SetText("More")
	button:SetClickedListener("OnMore")

	button=FtkButton.Create(win, 2*width/3, 3 * height/4 + 20, width/3-10, 60)
	button:SetText("Quit")
	button:SetClickedListener("OnQuit")

	win:SetFocus(button)
	win:ShowAll(1)

	return 1
end

Ftk.Init(1, {"listview"})
AppInit()
Ftk.Run()

