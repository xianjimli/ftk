function OnQuit(button)
	print("OnQuit: " .. button:GetText())
	Ftk.Quit()

	return RET_OK
end

function OnScroll(scrollbar)
	print("current value: " .. scrollbar:GetValue())
	return RET_OK
end

function AppInit()
	win=FtkAppWindow.Create()
	win:SetAttr(FTK_ATTR_QUIT_WHEN_CLOSE)
	win:SetText("Demo scrollbar")

	width=win:Width()
	height=win:Height()

	scroll_bar=FtkScrollBar.Create(win, width/8, 5, 0, height/2)
	scroll_bar:SetParam(0, 120, 120)
	scroll_bar:SetListener("OnScroll")
	
	scroll_bar=FtkScrollBar.Create(win, width/4, 5, 0, height/2)
	scroll_bar:SetParam(40, 120, 60)
	scroll_bar:SetListener("OnScroll")
	
	scroll_bar=FtkScrollBar.Create(win, 3*width/8, 5, 0, height/2)
	scroll_bar:SetParam(110, 120, 30)
	scroll_bar:SetListener("OnScroll")
	
	scroll_bar=FtkScrollBar.Create(win, width/2, 5, 0, height/2)
	scroll_bar:SetParam(120, 120, 20)
	scroll_bar:SetListener("OnScroll")
	
	-- h

	scroll_bar=FtkScrollBar.Create(win, 5, height/2 + 10, width - 10, 0)
	scroll_bar:SetParam(0, 120, 120)
	scroll_bar:SetListener("OnScroll")
	
	scroll_bar=FtkScrollBar.Create(win, 5, height/2 + 30, width - 10, 0)
	scroll_bar:SetParam(40, 120, 60)
	scroll_bar:SetListener("OnScroll")
	
	scroll_bar=FtkScrollBar.Create(win, 5, height/2 + 50, width - 10, 0)
	scroll_bar:SetParam(110, 120, 30)
	scroll_bar:SetListener("OnScroll")
	
	scroll_bar=FtkScrollBar.Create(win, 5, height/2 + 80, width - 10, 0)
	scroll_bar:SetParam(120, 120, 20)
	scroll_bar:SetListener("OnScroll")

	button=FtkButton.Create(win, 2*width/3, height/4, width/3-5, 50)
	button:SetText("Quit")
	button:SetClickedListener("OnQuit")

	win:SetFocus(button)
	win:ShowAll(1)

	return 1
end

Ftk.Init(1, {"scroll_bar"})
AppInit()
Ftk.Run()

