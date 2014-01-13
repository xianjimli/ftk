function OnQuit(button)
	button:Toplevel():Unref();
	return RET_OK
end

function OnNormalQuit(button)
	print("OnNormalQuit")
	button:Toplevel():Unref();
	return RET_OK;
end

function OnModalQuit(button)
	print("OnModalQuit")
	return RET_QUIT;
end

function OnChoosed(index, str)
	local info=FtkFileInfo.Create();
	FtkFile.GetInfo(str, info);
	print(info.name .. " mime:" .. FtkFile.GetMimeType(str));

end

function CreateSingle()
	s=FtkFileBrowser.Create(FTK_FILE_BROWER_SINGLE_CHOOSER)
	s:SetChoosedHandler("OnChoosed")
	s:SetPath("/")
	s:Load()
	FtkWidget.ShowAll(s, 1)
end

function CreateMulti()
	s=FtkFileBrowser.Create(FTK_FILE_BROWER_MULTI_CHOOSER)
	s:SetChoosedHandler("OnChoosed")
	s:SetPath("/")
	s:Load()
	FtkWidget.ShowAll(s, 1)
end

function OnNormal(button)
	CreateSingle()
	return RET_OK
end

function OnModal(button)
	CreateMulti();
	return RET_OK
end

function AppInit()
	win=FtkAppWindow.Create()
	win:SetText("Demo lua app")

	local width=win:Width()
	local height=win:Height()

	button=FtkButton.Create(win, 0, height/6, width/3, 50)
	button:SetText("Norma")
	button:SetClickedListener("OnNormal")

	button=FtkButton.Create(win, 2*width/3, height/6, width/3, 50)
	button:SetText("Model")
	button:SetClickedListener("OnModal")

	button=FtkButton.Create(win, width/4, height/2, width/2, 60)
	button:SetText("quit")
	button:SetClickedListener("OnQuit")
	
	win:SetFocus(button)
	win:ShowAll(1)

	return 1
end

AppInit()

