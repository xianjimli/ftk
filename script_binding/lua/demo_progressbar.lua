app_win=nil

function OnQuit(button)
	Ftk.Quit()

	return RET_OK
end

function OnTimer(data)
	percent=0

	for i=1,3 do
		progress_bar=app_win:Lookup(i);
		percent = progress_bar:GetPercent()
		progress_bar:SetPercent(percent + 10)
	end

	print("percent:" .. percent)
	if percent >= 100 then
		return RET_REMOVE
	end

	return RET_OK;
end

function AppInit()
	win=FtkAppWindow.Create()
	app_win=win
	win:SetText("Demo progressbar")
	win:SetAttr(FTK_ATTR_QUIT_WHEN_CLOSE)

	width=win:Width()
	height=win:Height()

	progress_bar=FtkProgressBar.Create(win, 10, height/6, width-20, 20)
	progress_bar:SetId(1)
	progress_bar:SetPercent(20)
	
	progress_bar=FtkProgressBar.Create(win, 10, height/3, width-20, 20)
	progress_bar:SetId(2)
	progress_bar:SetPercent(20)
	
	progress_bar=FtkProgressBar.Create(win, 10, height/2, width-20, 20)
	progress_bar:SetId(3)
	progress_bar:SetPercent(20)

	button=FtkButton.Create(win, width/4, 3*height/4, width/2, 60)
	button:SetText("Quit")
	button:SetClickedListener("OnQuit")

	timer=FtkSourceTimer.Create(1000, "OnTimer")
	Ftk.DefaultMainLoop():AddSource(timer)

	win:ShowAll(1)

	return 1
end

Ftk.Init(1, {"progressbar"})
AppInit()
Ftk.Run()

