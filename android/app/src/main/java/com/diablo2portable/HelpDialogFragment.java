package com.diablo2portable;

import android.app.Dialog;
import android.app.DialogFragment;
import android.os.Bundle;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.view.Window;
import android.widget.Button;

public class HelpDialogFragment extends DialogFragment {
    
    @Override
    public Dialog onCreateDialog(Bundle savedInstanceState) {
        Dialog dialog = super.onCreateDialog(savedInstanceState);
        
        // Remove title bar
        dialog.getWindow().requestFeature(Window.FEATURE_NO_TITLE);
        
        return dialog;
    }
    
    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
                             Bundle savedInstanceState) {
        View view = inflater.inflate(R.layout.dialog_help, container, false);
        
        Button closeButton = view.findViewById(R.id.close_help_button);
        if (closeButton != null) {
            closeButton.setOnClickListener(v -> dismiss());
        }
        
        return view;
    }
    
    @Override
    public void onStart() {
        super.onStart();
        
        // Set dialog width to fill screen
        Dialog dialog = getDialog();
        if (dialog != null) {
            int width = ViewGroup.LayoutParams.MATCH_PARENT;
            int height = ViewGroup.LayoutParams.WRAP_CONTENT;
            dialog.getWindow().setLayout(width, height);
        }
    }
}