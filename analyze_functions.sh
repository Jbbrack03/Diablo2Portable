#!/bin/bash

# Script to analyze C++ functions and find those longer than 30 lines
# Usage: ./analyze_functions.sh

find_long_functions() {
    local file="$1"
    local temp_file=$(mktemp)
    
    # Extract function signatures and their line numbers
    grep -n "^[a-zA-Z_][a-zA-Z0-9_]*::[a-zA-Z_][a-zA-Z0-9_]*(" "$file" | while read line; do
        local line_num=$(echo "$line" | cut -d: -f1)
        local func_sig=$(echo "$line" | cut -d: -f2-)
        
        # Find the opening brace
        local brace_line=$(tail -n +$line_num "$file" | grep -n "{" | head -1 | cut -d: -f1)
        if [ -n "$brace_line" ]; then
            local start_line=$((line_num + brace_line - 1))
            
            # Find the matching closing brace
            local brace_count=0
            local end_line=0
            local current_line=$start_line
            
            while IFS= read -r file_line; do
                current_line=$((current_line + 1))
                
                # Count braces (simplified - doesn't handle strings/comments perfectly)
                local open_braces=$(echo "$file_line" | grep -o "{" | wc -l)
                local close_braces=$(echo "$file_line" | grep -o "}" | wc -l)
                
                brace_count=$((brace_count + open_braces - close_braces))
                
                if [ $brace_count -eq 0 ]; then
                    end_line=$current_line
                    break
                fi
            done < <(tail -n +$start_line "$file")
            
            if [ $end_line -gt 0 ]; then
                local func_length=$((end_line - start_line + 1))
                if [ $func_length -gt 30 ]; then
                    echo "$file:$func_sig:$start_line:$end_line:$func_length"
                fi
            fi
        fi
    done
}

# Analyze all cpp files
find engine/src -name "*.cpp" -type f | while read file; do
    find_long_functions "$file"
done | sort -t: -k5 -nr