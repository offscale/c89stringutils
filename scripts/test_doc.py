import re

def get_doc_coverage():
    headers = [
        'c89stringutils/c89stringutils_string_extras.h',
        'c89stringutils/c89stringutils_safecrt.h',
        'c89stringutils/c89stringutils_log.h',
    ]
    total = 0
    documented = 0
    for h in headers:
        with open(h, 'r') as f:
            content = f.read()
        parts = re.split(r'(?:extern\s+)?C89STRINGUTILS_EXPORT\b', content)
        file_total = len(parts) - 1
        total += file_total
        for i in range(file_total):
            text_before = parts[i]
            last_comment_end = text_before.rfind('*/')
            if last_comment_end != -1:
                last_comment_start = text_before.rfind('/**', 0, last_comment_end)
                if last_comment_start != -1:
                    documented += 1
    return f"{(documented/total)*100:.2f}%"

print(get_doc_coverage())
