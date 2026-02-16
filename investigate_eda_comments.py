#!/usr/bin/env python3
"""
Investigation script to analyze comments in the enhanced_drone_analyzer directory.
"""

import os
import re
import json
from collections import defaultdict


class CommentAnalyzer:
    def __init__(self, directory):
        self.directory = directory
        self.results = {
            'total_files': 0,
            'files_with_comments': 0,
            'comment_stats': defaultdict(int),
            'languages': defaultdict(int),
            'legacy_code': defaultdict(int),
            'file_details': []
        }
        
        # Comment patterns
        self.single_line_comment = re.compile(r'//.*')
        self.multi_line_comment = re.compile(r'/\*[\s\S]*?\*/')
        self.legacy_code_pattern = re.compile(r'//.*?(?:\b(?:old|legacy|deprecated|todo|fixme|xxx|hack)\b|/\*.*?(?:old|legacy|deprecated|todo|fixme|xxx|hack).*?\*/)', re.IGNORECASE)
        
        # Non-English character detection (focus on Cyrillic, Chinese, Arabic, etc.)
        self.non_english_pattern = re.compile(r'[^\x00-\x7F]+')

    def analyze_file(self, filepath):
        """Analyze a single file for comments and legacy code."""
        try:
            with open(filepath, 'r', encoding='utf-8') as f:
                content = f.read()
        except Exception as e:
            print(f"Error reading {filepath}: {e}")
            return

        filename = os.path.basename(filepath)
        file_result = {
            'filename': filename,
            'path': filepath,
            'single_line_comments': 0,
            'multi_line_comments': 0,
            'documentation_comments': 0,
            'non_english_comments': 0,
            'legacy_code_comments': 0,
            'total_comment_chars': 0,
            'comment_lines': 0,
            'non_english_comment_text': [],
            'legacy_code_snippets': []
        }

        # Extract comments
        comments = []
        
        # Find multi-line comments first
        for match in self.multi_line_comment.finditer(content):
            comment_text = match.group()
            comments.append(comment_text)
            file_result['multi_line_comments'] += 1
            if '/**' in comment_text:
                file_result['documentation_comments'] += 1
            
            # Check for non-English comments
            non_english_matches = list(self.non_english_pattern.finditer(comment_text))
            if non_english_matches:
                file_result['non_english_comments'] += 1
                for m in non_english_matches:
                    file_result['non_english_comment_text'].append(m.group())
            
            # Check for legacy code comments
            if self.legacy_code_pattern.search(comment_text):
                file_result['legacy_code_comments'] += 1
                file_result['legacy_code_snippets'].append(comment_text)

        # Find single-line comments
        for match in self.single_line_comment.finditer(content):
            comment_text = match.group()
            comments.append(comment_text)
            file_result['single_line_comments'] += 1
            
            # Check for non-English comments
            non_english_matches = list(self.non_english_pattern.finditer(comment_text))
            if non_english_matches:
                file_result['non_english_comments'] += 1
                for m in non_english_matches:
                    file_result['non_english_comment_text'].append(m.group())
            
            # Check for legacy code comments
            if self.legacy_code_pattern.search(comment_text):
                file_result['legacy_code_comments'] += 1
                file_result['legacy_code_snippets'].append(comment_text)

        # Calculate comment statistics
        file_result['total_comment_chars'] = sum(len(comment) for comment in comments)
        file_result['comment_lines'] = sum(comment.count('\n') + 1 for comment in comments)

        # Determine comment languages
        if file_result['non_english_comments'] > 0:
            self.results['languages']['Non-English'] += file_result['non_english_comments']
        if file_result['single_line_comments'] + file_result['multi_line_comments'] > 0:
            self.results['languages']['English'] += (file_result['single_line_comments'] + file_result['multi_line_comments'] - file_result['non_english_comments'])

        # Update overall results
        self.results['total_files'] += 1
        if file_result['single_line_comments'] + file_result['multi_line_comments'] > 0:
            self.results['files_with_comments'] += 1
        
        self.results['comment_stats']['single_line'] += file_result['single_line_comments']
        self.results['comment_stats']['multi_line'] += file_result['multi_line_comments']
        self.results['comment_stats']['documentation'] += file_result['documentation_comments']
        self.results['comment_stats']['total'] += file_result['single_line_comments'] + file_result['multi_line_comments']
        
        self.results['legacy_code']['files'] += 1 if file_result['legacy_code_comments'] > 0 else 0
        self.results['legacy_code']['total'] += file_result['legacy_code_comments']

        self.results['file_details'].append(file_result)

    def analyze_directory(self):
        """Analyze all relevant files in the directory."""
        print(f"Analyzing directory: {self.directory}")
        
        for filename in os.listdir(self.directory):
            if filename.endswith('.cpp') or filename.endswith('.hpp'):
                filepath = os.path.join(self.directory, filename)
                print(f"Analyzing: {filename}")
                self.analyze_file(filepath)
        
        print("\nAnalysis complete!")

    def generate_report(self, output_path):
        """Generate a comprehensive report of the findings."""
        with open(output_path, 'w', encoding='utf-8') as f:
            f.write("# Enhanced Drone Analyzer Comment Investigation Report\n\n")
            
            f.write("## Summary Statistics\n")
            f.write("-------------------\n\n")
            
            f.write(f"- **Total Files Analyzed:** {self.results['total_files']}\n")
            f.write(f"- **Files with Comments:** {self.results['files_with_comments']} ({self.results['files_with_comments']/self.results['total_files']*100:.1f}%)\n")
            f.write(f"- **Total Comments:** {self.results['comment_stats']['total']}\n")
            f.write(f"- **Single-line Comments:** {self.results['comment_stats']['single_line']} ({self.results['comment_stats']['single_line']/self.results['comment_stats']['total']*100:.1f}%)\n")
            f.write(f"- **Multi-line Comments:** {self.results['comment_stats']['multi_line']} ({self.results['comment_stats']['multi_line']/self.results['comment_stats']['total']*100:.1f}%)\n")
            f.write(f"- **Documentation Comments:** {self.results['comment_stats']['documentation']} ({self.results['comment_stats']['documentation']/self.results['comment_stats']['total']*100:.1f}%)\n")
            f.write(f"- **Files with Legacy Code Comments:** {self.results['legacy_code']['files']}\n")
            f.write(f"- **Total Legacy Code Comments:** {self.results['legacy_code']['total']}\n")
            
            f.write("\n## Comment Language Distribution\n")
            f.write("---------------------------\n\n")
            
            total_comments = sum(self.results['languages'].values())
            for lang, count in self.results['languages'].items():
                percentage = count / total_comments * 100 if total_comments > 0 else 0
                f.write(f"- **{lang}:** {count} comments ({percentage:.1f}%)\n")
            
            f.write("\n## Detailed File Analysis\n")
            f.write("----------------------\n\n")
            
            for file_result in sorted(self.results['file_details'], key=lambda x: x['filename']):
                f.write(f"### {file_result['filename']}\n")
                f.write(f"- **Single-line Comments:** {file_result['single_line_comments']}\n")
                f.write(f"- **Multi-line Comments:** {file_result['multi_line_comments']}\n")
                f.write(f"- **Documentation Comments:** {file_result['documentation_comments']}\n")
                f.write(f"- **Non-English Comments:** {file_result['non_english_comments']}\n")
                f.write(f"- **Legacy Code Comments:** {file_result['legacy_code_comments']}\n")
                f.write(f"- **Comment Lines:** {file_result['comment_lines']}\n")
                f.write(f"- **Comment Characters:** {file_result['total_comment_chars']}\n")
                
                if file_result['non_english_comment_text']:
                    f.write(f"\n  **Non-English Comment Text:**\n")
                    for text in set(file_result['non_english_comment_text']):  # Deduplicate
                        f.write(f"  - {text}\n")
                
                if file_result['legacy_code_snippets']:
                    f.write(f"\n  **Legacy Code Snippets:**\n")
                    for snippet in file_result['legacy_code_snippets']:
                        snippet = snippet.strip()
                        if len(snippet) > 100:
                            snippet = snippet[:100] + "..."
                        f.write(f"  - {snippet}\n")
                
                f.write("\n")
            
            f.write("\n## Patterns and Issues Identified\n")
            f.write("---------------------------\n\n")
            
            issues = []
            
            if self.results['legacy_code']['total'] > 0:
                issues.append(f"{self.results['legacy_code']['total']} legacy code comments detected (marked as old, legacy, deprecated, todo, fixme, xxx, or hack)")
            
            if self.results['languages'].get('Non-English', 0) > 0:
                issues.append(f"{self.results['languages']['Non-English']} non-English comments detected (requires translation to English)")
            
            if self.results['comment_stats']['single_line'] / self.results['comment_stats']['total'] * 100 > 80:
                issues.append("High percentage of single-line comments - consider more multi-line or documentation comments for complex code")
            
            if self.results['comment_stats']['documentation'] / self.results['comment_stats']['total'] * 100 < 10:
                issues.append("Low percentage of documentation comments - consider adding more structured documentation")
            
            if not issues:
                f.write("No significant issues detected. Comments are well-structured and primarily in English.")
            else:
                for issue in issues:
                    f.write(f"- {issue}\n")

        print(f"Report generated at: {output_path}")


if __name__ == "__main__":
    directory = "firmware/application/apps/enhanced_drone_analyzer"
    analyzer = CommentAnalyzer(directory)
    analyzer.analyze_directory()
    analyzer.generate_report("EDA_COMMENT_INVESTIGATION_REPORT.md")
