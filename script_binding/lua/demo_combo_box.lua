function OnQuit(button)
	for i=1, 4 do
		combo_box=button:Toplevel():Lookup(i)
		print(tostring(i) .. ":" .. combo_box:GetText())
	end
	Ftk.Quit()

	return RET_OK
end

function ComboBoxInit(combo_box, n)
	for i=1,n do
		combo_box:Append(nil, i .. " seconds");
	end
end

function AppInit()
	win=FtkAppWindow.Create()
	win:SetText("Demo combobox")
	win:SetAttr(FTK_ATTR_QUIT_WHEN_CLOSE)

	width=win:Width()
	height=win:Height()

	width = width/2 - 10;
	combo_box=FtkComboBox.Create(win, 0, height/4, width, 50)
	combo_box:SetId(1)
	combo_box:SetText("1 second")
	ComboBoxInit(combo_box, 10)

	combo_box=FtkComboBox.Create(win, width + 10, height/4, width, 50);
	combo_box:SetId(2)
	combo_box:SetText("2 second")
	ComboBoxInit(combo_box, 3)

	button=FtkButton.Create(win, width/2, height/2, width, 60);
	button:SetText("Quit")
	button:SetClickedListener("OnQuit")

	combo_box=FtkComboBox.Create(win, 0, 3*height/4+5, width, 50)
	combo_box:SetId(3)
	combo_box:SetText("3 second")
	ComboBoxInit(combo_box, 3)

	combo_box=FtkComboBox.Create(win, width + 10, 3*height/4+5, width, 50);
	combo_box:SetId(4)
	combo_box:SetText("4 second")
	ComboBoxInit(combo_box, 30)

	win:SetFocus(button)
	win:ShowAll(1)

	return 1
end

Ftk.Init(1, {"combobox"})
AppInit()
Ftk.Run()

