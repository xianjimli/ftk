package org.libftk.app;

import java.io.InputStream;

import javax.microedition.khronos.egl.*;

import android.app.*;
import android.content.*;
import android.view.*;
import android.view.inputmethod.BaseInputConnection;
import android.view.inputmethod.CompletionInfo;
import android.view.inputmethod.EditorInfo;
import android.view.inputmethod.ExtractedText;
import android.view.inputmethod.ExtractedTextRequest;
import android.view.inputmethod.InputConnection;
import android.view.inputmethod.InputMethodManager;
import android.os.*;
import android.util.Log;
import android.text.method.*;
import android.text.*;
import android.content.res.AssetFileDescriptor;
import android.content.res.AssetManager;
import android.content.res.Configuration;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Canvas;
import android.graphics.Rect;

public class FtkActivity extends Activity {
    private static final boolean DEBUG = true;
    private static final String TAG = "FtkActivity";

    private static FtkActivity mActivity;
    private static FtkView mView;
    private static AssetManager mAssetManager;
    public static Thread mThread;

    static {
        System.loadLibrary("ftkapp");
    }

    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        
        mActivity = this;

        Window window = getWindow();
        window.requestFeature(Window.FEATURE_NO_TITLE);
        //requestWindowFeature(Window.FEATURE_NO_TITLE);
        window.setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN, WindowManager.LayoutParams.FLAG_FULLSCREEN);

        mAssetManager = getAssets();

        mView = new FtkView(this, getApplication());
        setContentView(mView);
        SurfaceHolder holder = mView.getHolder();
        holder.setType(SurfaceHolder.SURFACE_TYPE_GPU);

        //ActivityInfo.SCREEN_ORIENTATION_LANDSCAPE
        //ActivityInfo.SCREEN_ORIENTATION_PORTRAIT
        //getRequestedOrientation();
        //setRequestedOrientation();

        //List<InputMethodInfo> imInfos = imm.getEnabledInputMethodList();
        //Iterator<InputMethodInfo> it = imInfos.iterator();
        //while (it.hasNext()) {
        //	InputMethodInfo info = it.next();
        //	Log.d("FtkActivity", "InputMethod: " + info.getPackageName()
        //			+ ", " + info.loadLabel(getPackageManager()));
        //}
    }

    //Events
    protected void onPause()
    {
        super.onPause();
    }

    protected void onResume()
    {
        super.onResume();
    }
    
    protected void onStop() {
    	super.onStop();
    	if (DEBUG) { Log.d(TAG, "onStop"); }
    }

    protected void onDestroy() {
    	super.onDestroy();
    	if (DEBUG) { Log.d(TAG, "onDestroy"); }

    	nativeQuit();

        try {
        	mThread.wait();
        } catch (Exception e) {
            Log.e(TAG, "stopping ftk thread: " + e);
        }

        mAssetManager.close();

        ActivityManager am = (ActivityManager) getSystemService(Context.ACTIVITY_SERVICE);
        am.restartPackage(getPackageName());
    }

    public void onConfigurationChanged(Configuration newConfig) {
    	if (DEBUG) { Log.d(TAG, "onConfigurationChanged"); }
    	super.onConfigurationChanged(newConfig);
    	if (newConfig.orientation == Configuration.ORIENTATION_LANDSCAPE) {
    		Log.d(TAG, "ORIENTATION_LANDSCAPE");
    	} else if (newConfig.orientation == Configuration.ORIENTATION_PORTRAIT) {
    		Log.d(TAG, "ORIENTATION_PORTRAIT");
    	} else if (newConfig.orientation == Configuration.ORIENTATION_SQUARE) {
    		Log.d(TAG, "ORIENTATION_SQUARE");
    	}
    }


    public static native void nativeInit();
    public static native void nativeQuit();
    public static native void nativeSetScreenSize(int width, int height);
    public static native void nativeEnableRender();
    public static native void nativeDisableRender();
    public static native void onNativeKey(int action, int keyCode);
    public static native void onNativeCommitText(String text, int newCursorPosition);
    public static native void onNativeTouch(int action, float x, float y, float p);
    public static native void onNativeResize(int x, int y, int format);


    // Java functions called from C
    
    public static final AssetFileDescriptor getAssetFileDescriptor(String file) {
    	if (DEBUG) { Log.d(TAG, "getAssetFileDescriptor: " + file); }
    	try {
    	    return mAssetManager.openFd(file);
    	} catch (Exception ex) {
    		Log.e(TAG, ex.getMessage());
    	}
    	return null;
    }
    
    public static final Bitmap decodeImage(String file) {
    	//if (DEBUG) { Log.d(TAG, "decodeImage: " + file); }
    	InputStream is = null;
    	try {
    		is = mAssetManager.open(file);
    	} catch (Exception ex) {
    		Log.e(TAG, ex.getMessage());
    		return null;
    	}
    	Bitmap bitmap = BitmapFactory.decodeStream(is);
    	try {
    		is.close();
    	} catch (Exception ex) {
    		Log.e(TAG, ex.getMessage());
    	}
    	is = null;
    	return bitmap;
    }

    public static final void initEGL() {
        mView.initEGL();
    }
    
    public static final void createEGLSurface() {
    	mView.createEGLSurface();
    }
    
    public static final void destroyEGLSurface() {
    	mView.destroyEGLSurface();
    }

    public static final void flipEGL() {
        mView.flipEGL();
    }
    
    public static final void drawBitmap(int[] colors, int offset, int stride, int x, int y, int width, int height) {
    	mView.drawBitmap(colors, offset, stride, x, y, width, height);
    }

    public static final void showKeyboard() {
    	InputMethodManager imm = (InputMethodManager) mActivity.getSystemService(INPUT_METHOD_SERVICE);
    	imm.showSoftInput(mView, 0);
    }

    public static final void hideKeyboard() {
    	InputMethodManager imm = (InputMethodManager) mActivity.getSystemService(INPUT_METHOD_SERVICE);
    	imm.hideSoftInputFromWindow(mView.getWindowToken(), 0);
    }

    public static final void showInputMethodPicker() {
    	InputMethodManager imm = (InputMethodManager) mActivity.getSystemService(INPUT_METHOD_SERVICE);
    	//imm.hideSoftInputFromWindow(mView.getWindowToken(), 0);
    	imm.showInputMethodPicker();
    }
}

class FtkRunner implements Runnable {
    private static final boolean DEBUG = true;
    private static final String TAG = "FtkRunner";

    private FtkActivity mActivity;

    public FtkRunner(FtkActivity activity) {
    	super();
    	mActivity = activity;
    }

    public void run() {
        if (DEBUG) { Log.d(TAG, "thread run"); }

        FtkActivity.nativeInit();

        if (DEBUG) { Log.v(TAG, "thread exit"); }

        mActivity.finish();
    }
}

class FtkInputConnection extends BaseInputConnection {
    private static final boolean DEBUG = true;
    private static final String TAG = "FtkInputConnection";

    private final FtkView mView;

    public FtkInputConnection(FtkView view) {
        super(view, true);
        mView = view;
    }
	
    public Editable getEditable() {
        FtkView v = mView;
        if (v != null) {
            return v.getEditableText();
        }
        return null;
    }
    
    public boolean beginBatchEdit() {
        mView.beginBatchEdit();
        return true;
    }
    
    public boolean endBatchEdit() {
        mView.endBatchEdit();
        return true;
    }
    
    public boolean clearMetaKeyStates(int states) {
        final Editable content = getEditable();
        if (content == null) return false;
        KeyListener kl = mView.getKeyListener();
        if (kl != null) {
            try {
                kl.clearMetaKeyState(mView, content, states);
            } catch (AbstractMethodError e) {
                // This is an old listener that doesn't implement the
                // new method.
            }
        }
        return true;
    }
    
    public boolean commitCompletion(CompletionInfo text) {
        if (DEBUG) Log.v(TAG, "commitCompletion " + text);
        mView.beginBatchEdit();
        mView.onCommitCompletion(text);
        mView.endBatchEdit();
        return true;
    }

    public boolean performEditorAction(int actionCode) {
        if (DEBUG) Log.v(TAG, "performEditorAction " + actionCode);
        mView.onEditorAction(actionCode);
        return true;
    }
    
    public boolean performContextMenuAction(int id) {
        if (DEBUG) Log.v(TAG, "performContextMenuAction " + id);
        mView.beginBatchEdit();
        mView.onTextContextMenuItem(id);
        mView.endBatchEdit();
        return true;
    }
    
    public ExtractedText getExtractedText(ExtractedTextRequest request, int flags) {
        if (mView != null) {
            ExtractedText et = new ExtractedText();
            if (mView.extractText(request, et)) {
                if ((flags&GET_EXTRACTED_TEXT_MONITOR) != 0) {
                    mView.setExtracting(request);
                }
                return et;
            }
        }
        return null;
    }
    
    public boolean performPrivateCommand(String action, Bundle data) {
        mView.onPrivateIMECommand(action, data);
        return true;
    }

    @Override
    public boolean commitText(CharSequence text, int newCursorPosition) {
    	if (DEBUG) Log.v(TAG, "commitText " + text);
        if (mView == null) {
            return super.commitText(text, newCursorPosition);
        }
        
        FtkActivity.onNativeCommitText(String.valueOf(text), newCursorPosition);

        boolean success = super.commitText(text, newCursorPosition);
        return success;
    }
}

class FtkView extends SurfaceView implements SurfaceHolder.Callback, View.OnKeyListener, View.OnTouchListener {

    private static final boolean DEBUG = true;
    private static final String TAG = "FtkView";

    private FtkActivity mActivity;
	
    private boolean mSurfaceValid = false;

    private EGLDisplay mEGLDisplay;
    private EGLConfig mEGLConfig;
    private EGLContext mEGLContext;
    private EGLSurface mEGLSurface;

    public FtkView(FtkActivity activity, Context context) {
        super(context);
        mActivity = activity;

        SurfaceHolder holder = getHolder();
        holder.addCallback(this);

        setFocusable(true);
        setFocusableInTouchMode(true);
        requestFocus();
        setOnKeyListener(this);
        setOnTouchListener(this);

        mText = "";
    }

    public void surfaceCreated(SurfaceHolder holder) {
        if (DEBUG) { Log.v(TAG, "surfaceCreated"); }

        if (FtkActivity.mThread != null) {
            mSurfaceValid = true;
            FtkActivity.nativeEnableRender();
            return;
        }

        int width = getWidth();
        int height = getHeight();
        FtkActivity.nativeSetScreenSize(width, height);

        FtkActivity.mThread = new Thread(new FtkRunner(mActivity), "ftk thread");
        FtkActivity.mThread.start();

        mSurfaceValid = true;
        FtkActivity.nativeEnableRender();
    }

    public void surfaceDestroyed(SurfaceHolder holder) {
        if (DEBUG) { Log.v(TAG, "surfaceDestroyed"); }
        mSurfaceValid = false;
        FtkActivity.nativeDisableRender();
    }

    public void surfaceChanged(SurfaceHolder holder, int format, int width, int height) {
        if (DEBUG) { Log.d(TAG, "surfaceChanged"); }
        FtkActivity.onNativeResize(width, height, format);
    }


    // EGL functions
    public boolean initEGL() {
        if (DEBUG) { Log.d(TAG, "initEGL"); }

        try {
            EGL10 egl = (EGL10)EGLContext.getEGL();

            EGLDisplay dpy = egl.eglGetDisplay(EGL10.EGL_DEFAULT_DISPLAY);
            int[] version = new int[2];
            egl.eglInitialize(dpy, version);

            int[] configSpec = {
		        EGL10.EGL_RED_SIZE, 5,
		        EGL10.EGL_GREEN_SIZE, 6,
		        EGL10.EGL_BLUE_SIZE, 5,
		        EGL10.EGL_ALPHA_SIZE, EGL10.EGL_DONT_CARE,
		        EGL10.EGL_DEPTH_SIZE, 16,
		        EGL10.EGL_STENCIL_SIZE, EGL10.EGL_DONT_CARE,
		        EGL10.EGL_SURFACE_TYPE, EGL10.EGL_WINDOW_BIT,
                EGL10.EGL_NONE
            };
            EGLConfig[] configs = new EGLConfig[1];
            int[] num_config = new int[1];
            egl.eglChooseConfig(dpy, configSpec, configs, 1, num_config);
            EGLConfig config = configs[0];

            EGLContext ctx = egl.eglCreateContext(dpy, config, EGL10.EGL_NO_CONTEXT, null);

            mEGLDisplay = dpy;
            mEGLConfig = config;
            mEGLContext = ctx;

        } catch (Exception e) {
            Log.e(TAG, e + "");
            for(StackTraceElement s : e.getStackTrace()) {
                Log.e(TAG, s.toString());
            }
        }

        return true;
    }
    
    public void createEGLSurface() {
    	if (DEBUG) { Log.d(TAG, "createEGLSurface"); }
    	EGL10 egl = (EGL10)EGLContext.getEGL();
    	//if (mEGLSurface != null) {
    		// xxx
    	//}

    	EGLSurface surface = egl.eglCreateWindowSurface(mEGLDisplay, mEGLConfig, this, null);
        egl.eglMakeCurrent(mEGLDisplay, surface, surface, mEGLContext);

        mEGLSurface = surface;
    }

    public void destroyEGLSurface() {
    	if (DEBUG) { Log.d(TAG, "destroyEGLSurface"); }
    	//EGL10 egl = (EGL10)EGLContext.getEGL();
    	if (mEGLSurface != null) {
    		//egl.eglMakeCurrent(mEGLDisplay, EGL10.EGL_NO_SURFACE, EGL10.EGL_NO_SURFACE, mEGLContext);
    		//egl.eglDestroySurface(mEGLDisplay, mEGLSurface);
    		mEGLSurface = null;
    	}
    }

    public void flipEGL() {
    	if (!mSurfaceValid) {
        	return;
        }
        try {
            EGL10 egl = (EGL10)EGLContext.getEGL();

            egl.eglWaitNative(EGL10.EGL_NATIVE_RENDERABLE, null);
            egl.eglWaitGL();
            egl.eglSwapBuffers(mEGLDisplay, mEGLSurface);
            
        } catch(Exception e) {
            Log.v("FTK", "flipEGL(): " + e);

            for(StackTraceElement s : e.getStackTrace()){
                Log.v("FTK", s.toString());
            }
        }
    }
    
    public void drawBitmap(int[] colors, int offset, int stride, int x, int y, int width, int height) {
    	//if (DEBUG) { Log.d(TAG, "drawBitmap: " + x + "-" + y + " " + width + "-" + height); }
    	SurfaceHolder holder = getHolder();
    	if (holder == null) {
    		return;
    	}
    	Rect rect = new Rect(x, y, x + width, y + height);
    	Canvas canvas = holder.lockCanvas(rect);
    	if (canvas == null) {
    		return;
    	}
    	canvas.drawBitmap(colors, offset, stride, x, y, width, height, true, null);
    	holder.unlockCanvasAndPost(canvas);
    }

    public boolean onKey(View  v, int keyCode, KeyEvent event){
    	FtkActivity.onNativeKey(event.getAction(), keyCode);
        return true;
    }

    public boolean onTouch(View v, MotionEvent event){
        FtkActivity.onNativeTouch(event.getAction(), event.getX(), event.getY(), event.getPressure());
        return true;
    }


    // input method

    /**
     * Interface definition for a callback to be invoked when an action is
     * performed on the editor.
     */
    public interface OnEditorActionListener {
        /**
         * Called when an action is being performed.
         *
         * @param v The view that was clicked.
         * @param actionId Identifier of the action.  This will be either the
         * identifier you supplied, or {@link EditorInfo#IME_NULL
         * EditorInfo.IME_NULL} if being called due to the enter key
         * being pressed.
         * @param event If triggered by an enter key, this is the event;
         * otherwise, this is null.
         * @return Return true if you have consumed the action, else false.
         */
        boolean onEditorAction(FtkView v, int actionId, KeyEvent event);
    }
    
    class InputContentType {
        int imeOptions = EditorInfo.IME_NULL;
        String privateImeOptions;
        CharSequence imeActionLabel;
        int imeActionId;
        Bundle extras;
        OnEditorActionListener onEditorActionListener;
        boolean enterDown;
    }
    InputContentType mInputContentType;
    
    @Override public boolean onCheckIsTextEditor() {
    	return true;
    }
    
    @Override public InputConnection onCreateInputConnection(EditorInfo outAttrs) {
    	return new FtkInputConnection(this);
    }
    
    /**
     * Called when an attached input method calls
     * {@link InputConnection#performEditorAction(int)
     * InputConnection.performEditorAction()}
     * for this text view.  The default implementation will call your action
     * listener supplied to {@link #setOnEditorActionListener}, or perform
     * a standard operation for {@link EditorInfo#IME_ACTION_NEXT
     * EditorInfo.IME_ACTION_NEXT} or {@link EditorInfo#IME_ACTION_DONE
     * EditorInfo.IME_ACTION_DONE}.
     * 
     * <p>For backwards compatibility, if no IME options have been set and the
     * text view would not normally advance focus on enter, then
     * the NEXT and DONE actions received here will be turned into an enter
     * key down/up pair to go through the normal key handling.
     * 
     * @param actionCode The code of the action being performed.
     * 
     * @see #setOnEditorActionListener
     */
    public void onEditorAction(int actionCode) {
        /*final InputContentType ict = mInputContentType;
        if (ict != null) {
            if (ict.onEditorActionListener != null) {
                if (ict.onEditorActionListener.onEditorAction(this,
                        actionCode, null)) {
                    return;
                }
            }
            
            // This is the handling for some default action.
            // Note that for backwards compatibility we don't do this
            // default handling if explicit ime options have not been given,
            // instead turning this into the normal enter key codes that an
            // app may be expecting.
            if (actionCode == EditorInfo.IME_ACTION_NEXT) {
                View v = focusSearch(FOCUS_DOWN);
                if (v != null) {
                    if (!v.requestFocus(FOCUS_DOWN)) {
                        throw new IllegalStateException("focus search returned a view " +
                                "that wasn't able to take focus!");
                    }
                }
                return;
                
            } else if (actionCode == EditorInfo.IME_ACTION_DONE) {
            	FtkActivity activity = FtkActivity.mSingleton;
            	InputMethodManager imm = (InputMethodManager) activity.getSystemService(FtkActivity.INPUT_METHOD_SERVICE);
                if (imm != null) {
                    imm.hideSoftInputFromWindow(getWindowToken(), 0);
                }
                return;
            }
        }
        
        Handler h = getHandler();
        if (h != null) {
            long eventTime = SystemClock.uptimeMillis();
            h.sendMessage(h.obtainMessage(ViewRoot.DISPATCH_KEY_FROM_IME,
                    new KeyEvent(eventTime, eventTime,
                    KeyEvent.ACTION_DOWN, KeyEvent.KEYCODE_ENTER, 0, 0, 0, 0,
                    KeyEvent.FLAG_SOFT_KEYBOARD | KeyEvent.FLAG_KEEP_TOUCH_MODE
                    | KeyEvent.FLAG_EDITOR_ACTION)));
            h.sendMessage(h.obtainMessage(ViewRoot.DISPATCH_KEY_FROM_IME,
                    new KeyEvent(SystemClock.uptimeMillis(), eventTime,
                    KeyEvent.ACTION_UP, KeyEvent.KEYCODE_ENTER, 0, 0, 0, 0,
                    KeyEvent.FLAG_SOFT_KEYBOARD | KeyEvent.FLAG_KEEP_TOUCH_MODE
                    | KeyEvent.FLAG_EDITOR_ACTION)));
        }*/
    }
    
    /**
     * If this TextView contains editable content, extract a portion of it
     * based on the information in <var>request</var> in to <var>outText</var>.
     * @return Returns true if the text was successfully extracted, else false.
     */
    public boolean extractText(ExtractedTextRequest request,
            ExtractedText outText) {
        return extractTextInternal(request, EXTRACT_UNKNOWN, EXTRACT_UNKNOWN,
                EXTRACT_UNKNOWN, outText);
    }
    
    static final int EXTRACT_NOTHING = -2;
    static final int EXTRACT_UNKNOWN = -1;
    
    boolean extractTextInternal(ExtractedTextRequest request,
            int partialStartOffset, int partialEndOffset, int delta,
            ExtractedText outText) {
        /*final CharSequence content = mText;
        if (content != null) {
            if (partialStartOffset != EXTRACT_NOTHING) {
                final int N = content.length();
                if (partialStartOffset < 0) {
                    outText.partialStartOffset = outText.partialEndOffset = -1;
                    partialStartOffset = 0;
                    partialEndOffset = N;
                } else {
                    // Now use the delta to determine the actual amount of text
                    // we need.
                    partialEndOffset += delta;
                    // Adjust offsets to ensure we contain full spans.
                    if (content instanceof Spanned) {
                        Spanned spanned = (Spanned)content;
                        Object[] spans = spanned.getSpans(partialStartOffset,
                                partialEndOffset, ParcelableSpan.class);
                        int i = spans.length;
                        while (i > 0) {
                            i--;
                            int j = spanned.getSpanStart(spans[i]);
                            if (j < partialStartOffset) partialStartOffset = j;
                            j = spanned.getSpanEnd(spans[i]);
                            if (j > partialEndOffset) partialEndOffset = j;
                        }
                    }
                    outText.partialStartOffset = partialStartOffset;
                    outText.partialEndOffset = partialEndOffset - delta;

                    if (partialStartOffset > N) {
                        partialStartOffset = N;
                    } else if (partialStartOffset < 0) {
                        partialStartOffset = 0;
                    }
                    if (partialEndOffset > N) {
                        partialEndOffset = N;
                    } else if (partialEndOffset < 0) {
                        partialEndOffset = 0;
                    }
                }
                if ((request.flags&InputConnection.GET_TEXT_WITH_STYLES) != 0) {
                    outText.text = content.subSequence(partialStartOffset,
                            partialEndOffset);
                } else {
                    outText.text = TextUtils.substring(content, partialStartOffset,
                            partialEndOffset);
                }
            } else {
                outText.partialStartOffset = 0;
                outText.partialEndOffset = 0;
                outText.text = "";
            }
            outText.flags = 0;
            if (MetaKeyKeyListener.getMetaState(mText, MetaKeyKeyListener.META_SELECTING) != 0) {
                outText.flags |= ExtractedText.FLAG_SELECTING;
            }
            if (mSingleLine) {
                outText.flags |= ExtractedText.FLAG_SINGLE_LINE;
            }
            outText.startOffset = 0;
            outText.selectionStart = Selection.getSelectionStart(content);
            outText.selectionEnd = Selection.getSelectionEnd(content);
            return true;
        }*/
        return false;
    }
    
    /**
     * @hide
     */
    public void setExtracting(ExtractedTextRequest req) {
        /*if (mInputMethodState != null) {
            mInputMethodState.mExtracting = req;
        }*/
    }
    
    /**
     * Called by the framework in response to a private command from the
     * current method, provided by it calling
     * {@link InputConnection#performPrivateCommand
     * InputConnection.performPrivateCommand()}.
     *
     * @param action The action name of the command.
     * @param data Any additional data for the command.  This may be null.
     * @return Return true if you handled the command, else false.
     */
    public boolean onPrivateIMECommand(String action, Bundle data) {
        return false;
    }
    
    /**
     * Return the text the TextView is displaying as an Editable object.  If
     * the text is not editable, null is returned.
     *
     * @see #getText
     */
    public Editable getEditableText() {
        return (mText instanceof Editable) ? (Editable)mText : null;
    }
    
    /**
     * Called by the framework in response to a text completion from
     * the current input method, provided by it calling
     * {@link InputConnection#commitCompletion
     * InputConnection.commitCompletion()}.  The default implementation does
     * nothing; text views that are supporting auto-completion should override
     * this to do their desired behavior.
     *
     * @param text The auto complete text the user has selected.
     */
    public void onCommitCompletion(CompletionInfo text) {
    	if (DEBUG) Log.v(TAG, "onCommitCompletion " + text.getText());
    }
    
    /**
     * Called when a context menu option for the text view is selected.  Currently
     * this will be one of: {@link android.R.id#selectAll},
     * {@link android.R.id#startSelectingText}, {@link android.R.id#stopSelectingText},
     * {@link android.R.id#cut}, {@link android.R.id#copy},
     * {@link android.R.id#paste}, {@link android.R.id#copyUrl},
     * or {@link android.R.id#switchInputMethod}.
     */
    public boolean onTextContextMenuItem(int id) {
        /*int selStart = getSelectionStart();
        int selEnd = getSelectionEnd();

        if (!isFocused()) {
            selStart = 0;
            selEnd = mText.length();
        }

        int min = Math.min(selStart, selEnd);
        int max = Math.max(selStart, selEnd);

        if (min < 0) {
            min = 0;
        }
        if (max < 0) {
            max = 0;
        }

        ClipboardManager clip = (ClipboardManager)getContext()
                .getSystemService(Context.CLIPBOARD_SERVICE);

        switch (id) {
            case ID_SELECT_ALL:
                Selection.setSelection((Spannable) mText, 0,
                        mText.length());
                return true;

            case ID_START_SELECTING_TEXT:
                MetaKeyKeyListener.startSelecting(this, (Spannable) mText);
                return true;

            case ID_STOP_SELECTING_TEXT:
                MetaKeyKeyListener.stopSelecting(this, (Spannable) mText);
                Selection.setSelection((Spannable) mText, getSelectionEnd());
                return true;

            case ID_CUT:
                MetaKeyKeyListener.stopSelecting(this, (Spannable) mText);

                if (min == max) {
                    min = 0;
                    max = mText.length();
                }

                clip.setText(mTransformed.subSequence(min, max));
                ((Editable) mText).delete(min, max);
                return true;

            case ID_COPY:
                MetaKeyKeyListener.stopSelecting(this, (Spannable) mText);

                if (min == max) {
                    min = 0;
                    max = mText.length();
                }

                clip.setText(mTransformed.subSequence(min, max));
                return true;

            case ID_PASTE:
                MetaKeyKeyListener.stopSelecting(this, (Spannable) mText);

                CharSequence paste = clip.getText();

                if (paste != null) {
                    Selection.setSelection((Spannable) mText, max);
                    ((Editable) mText).replace(min, max, paste);
                }

                return true;

            case ID_COPY_URL:
                MetaKeyKeyListener.stopSelecting(this, (Spannable) mText);

                URLSpan[] urls = ((Spanned) mText).getSpans(min, max,
                                                       URLSpan.class);
                if (urls.length == 1) {
                    clip.setText(urls[0].getURL());
                }

                return true;

            case ID_SWITCH_INPUT_METHOD:
                InputMethodManager imm = InputMethodManager.peekInstance();
                if (imm != null) {
                    imm.showInputMethodPicker();
                }
                return true;

            case ID_ADD_TO_DICTIONARY:
                String word = getWordForDictionary();

                if (word != null) {
                    Intent i = new Intent("com.android.settings.USER_DICTIONARY_INSERT");
                    i.putExtra("word", word);
                    i.setFlags(i.getFlags() | Intent.FLAG_ACTIVITY_NEW_TASK);
                    getContext().startActivity(i);
                }

                return true;
            }*/

        return false;
    }
    
    public void beginBatchEdit() {
    	if (DEBUG) Log.v(TAG, "beginBatchEdit");
        /*final InputMethodState ims = mInputMethodState;
        if (ims != null) {
            int nesting = ++ims.mBatchEditNesting;
            if (nesting == 1) {
                ims.mCursorChanged = false;
                ims.mChangedDelta = 0;
                if (ims.mContentChanged) {
                    // We already have a pending change from somewhere else,
                    // so turn this into a full update.
                    ims.mChangedStart = 0;
                    ims.mChangedEnd = mText.length();
                } else {
                    ims.mChangedStart = EXTRACT_UNKNOWN;
                    ims.mChangedEnd = EXTRACT_UNKNOWN;
                    ims.mContentChanged = false;
                }
                onBeginBatchEdit();
            }
        }*/
    }
    
    public void endBatchEdit() {
    	if (DEBUG) Log.v(TAG, "endBatchEdit");
        /*final InputMethodState ims = mInputMethodState;
        if (ims != null) {
            int nesting = --ims.mBatchEditNesting;
            if (nesting == 0) {
                finishBatchEdit(ims);
            }
        }*/
    }
    
    /**
     * @return the current key listener for this TextView.
     * This will frequently be null for non-EditText TextViews.
     */
    public final KeyListener getKeyListener() {
        return mInput;
    }
    
    private CharSequence mText;
    private KeyListener mInput;
}
