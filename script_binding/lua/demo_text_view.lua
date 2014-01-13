function OnQuit(button)
	text_view=button:Toplevel():Lookup(1)
	print(text_view:GetText())

	Ftk.Quit()

	return RET_OK
end

function AppInit()
	win=FtkAppWindow.Create()
	win:SetText("Demo text view")
	win:SetAttr(FTK_ATTR_QUIT_WHEN_CLOSE)
	
	width=win:Width()
	height=win:Height()

	str="Multi line editor:\nfirst\nsecond\nthird\nforth\n012345667890qwertyuiopasdfghjklkzxcbnmcutukakdfasiwerksjdfaksdjfaksdjifiwjkaldfkjkalsdfjieirnlkkafjiierklaaa\n1\n2\n3\n\n4\n5\n6\n7\n8\nlast line"
	text_view=FtkTextView.Create(win, 10, 10, width - 20, height/3)
	text_view:SetId(1)
	text_view:SetText(str)
	
	text_view=FtkTextView.Create(win, 10, 15 + height/3, width - 20, height/3)
	text_view:SetId(2)
	text_view:SetText("abc")
	text_view:InsertText(3, str)
	text_view:SetReadonly(1)

	button=FtkButton.Create(win, width/4, 3*height/4, width/2, 60)
	button:SetText("Quit")
	button:SetClickedListener("OnQuit")
	
	win:SetFocus(button)
	win:ShowAll(1)

	return 1
end

Ftk.Init(1, {"textview"})
AppInit()
Ftk.Run()

