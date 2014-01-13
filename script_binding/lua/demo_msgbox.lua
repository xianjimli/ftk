function OnTips(button)
	ret=Ftk.Tips("The dialog will quit in 3 seconds.");
	return RET_OK
end

function OnWarning(button)
	ret=Ftk.Warning("Warning", 
	"December 31, 2008: patchwork.kernel.org is now available for general use. It is currently only monitoring the Linux Kernel mailing-list, but should be useful to kernel developers in dealing with patches flying across the wire.", 
	{"Yes", "No"})
	print("Ret=" .. ret)

	return RET_OK
end

function OnQuestion(button)
	ret=Ftk.Question("Question", "Are you sure to quit?",  {"Yes", "No", "Help"})
	print("Ret=" .. ret)
	return RET_OK
end

function OnInfomation(button)
	ret = Ftk.Infomation("Infomation", "September 19, 2008: mirrors.kernel.org has been flipped over to using our new GeoDNS based bind server (named-geodns). This means that, at the dns query level, our servers will attempt to direct you to the nearest / fastest kernel.org mirror for your request. This means that you no longer have to use mirrors.us.kernel.org or mirrors.eu.kernel.org to generally route you to the right place. This does mean a change to mirrors.kernel.org no longer explicitly pointing at mirrors.us.kernel.org. Additional information on named-geodns will be forth coming, check back here for an addendum soon.", {"OK"})
	print("Ret=" .. ret)

	return RET_OK
end


function AppInit()
	win=FtkAppWindow.Create()
	win:SetText("Demo messagebox")
	win:SetAttr(FTK_ATTR_QUIT_WHEN_CLOSE)

	width=win:Width()
	height=win:Height()

	button=FtkButton.Create(win, 0, height/6, width/3, 50)
	button:SetText("Tips")
	button:SetClickedListener("OnTips")

	button=FtkButton.Create(win, 2*width/3, height/6, width/3, 50)
	button:SetText("Warning")
	button:SetClickedListener("OnWarning")

	button=FtkButton.Create(win, 0, height/2, width/3, 50)
	button:SetText("Question")
	button:SetClickedListener("OnQuestion")

	button=FtkButton.Create(win, 2*width/3, height/2, width/3, 50)
	button:SetText("Infomation")
	button:SetClickedListener("OnInfomation")

	win:SetFocus(button)
	win:ShowAll(1)

	return 1
end

Ftk.Init(1, {"messagebox"})
AppInit()
Ftk.Run()

