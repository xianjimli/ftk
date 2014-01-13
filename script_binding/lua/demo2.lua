Ftk.Init(1, {"demo2"})

win=FtkAppWindow.Create()
win:SetAttr(FTK_ATTR_QUIT_WHEN_CLOSE)
win:SetText("Hello FTK!")
win:ShowAll(1)
Ftk.Run()


