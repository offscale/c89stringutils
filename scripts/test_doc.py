def get_doc_coverage():
    with open('c89stringutils/c89stringutils_string_extras.h', 'r') as f:
        content = f.read()
    parts = content.split('extern C89STRINGUTILS_EXPORT')
    total = len(parts) - 1
    documented = 0
    for i in range(total):
        text_before = parts[i]
        last_comment_end = text_before.rfind('*/')
        if last_comment_end != -1:
            last_comment_start = text_before.rfind('/**', 0, last_comment_end)
            if last_comment_start != -1:
                documented += 1
    return f"{(documented/total)*100:.2f}%"
print(get_doc_coverage())
