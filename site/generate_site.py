import os
import markdown
import codecs

md = markdown.Markdown(extensions=["markdown.extensions.fenced_code"])

def readall(path):
    f = codecs.open(path, "r",encoding="utf-8")
    text = "".join(f.readlines())
    f.close()
    return text

markdown_names = []

for _, _, files in os.walk("."):
    markdown_names = sorted(filter(lambda x: x.endswith(".markdown") == True, files))

template_text = readall("template.html")
template_data = template_text.split("{{body}}")

for n in markdown_names:
    source_text = readall(n)

    out_name = "../html_cache/" + n.rsplit(".", 2)[0] + ".html"
    f = codecs.open(out_name, "w", encoding="utf-8", errors="xmlcharrefreplace")
    f.write(template_data[0])
    f.write(md.convert(source_text))
    md.reset()
    f.write(template_data[1])
    f.close()
