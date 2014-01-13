function OnQuit(button)
	Ftk.Quit()
	print("button_quit_on_clicked")
	return 0
end

Ftk.Init(1, {"demo2"})
win=FtkAppWindow.Create()
w=win:Width()
h=win:Height()

label=FtkLabel.Create(win, 0, 0, w, 30);
label:SetText("Hello");

entry=FtkEntry.Create(win, 0, 50, w, 30);
entry:SetText("It is ok.");

button=FtkButton.Create(win, w/4, h/2, w/2, 50);
button:SetText("Quit")
button:SetClickedListener("OnQuit");

win:SetText("Hello FTK!")
win:ShowAll(1)
Ftk.Run()


