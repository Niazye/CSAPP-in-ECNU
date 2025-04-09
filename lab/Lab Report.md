---
title: This is Title
chrome:
    format: "A4"
    headerTemplate: '<div style="width:100%; color:#d5d5d5; text-align:center; border-bottom: 1pt solid #808080; margin: 20px 40px 40px; font-size: 10pt">     <span class=title></span></div>'
    footerTemplate: '<div style="width:100%; color:#d5d5d5; text-align:center; margin:  40px 40px 40px; font-size: 8pt;">- <span class=pageNumber></span> - </div>'
    displayHeaderFooter: true
    margin:
        top: '80px'
        bottom: '80px'
        left: '60px'
        right: '60px'
---

# Simple Demo of PDF 分页 页眉 页脚

## page 1

- text 1 
- text 1 
- text 1[^1]

[^1]: this is demo text 

`markdown分页` :

<div STYLE="page-break-after: always;"></div>

## page 2

- text 2
- text 2
- text 2
