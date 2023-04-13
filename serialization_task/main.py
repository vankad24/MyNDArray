from enum import Enum
import json
import zlib


class Alignment(Enum):
    HORIZONTAL = 1
    VERTICAL = 2
    
    @classmethod
    def from_dict(cls, d, parent):
        return Alignment.HORIZONTAL if d["value"] == 1 else Alignment.VERTICAL
    

class Widget():
    def __init__(self, parent):
        self.parent = parent
        self.children = []
        if self.parent is not None:
            self.parent.add_child(self)

    def add_child(self, children: "Widget"):
        self.children.append(children)

    def to_binary(self):
        def f(obj):
            if isinstance(obj,Alignment):
                return {"class":"Alignment","value":obj.value}
            else:
                d = obj.__dict__.copy()
                d["class"] = obj.__class__.__name__
                del d["parent"]
                return d
        return zlib.compress(bytes(json.dumps(app,default=f),"utf-8"))
    
    @classmethod
    def from_binary(cls, data):
        return cls.from_dict(json.loads(zlib.decompress(data)),None)

    @classmethod
    def from_dict(cls, d, parent):
        return Widget(None)

    def __str__(self):
        c = self.children
        return f"{self.__class__.__name__}{c if len(c)>0 else ''}"

    def __repr__(self):
        return str(self)

class MainWindow(Widget):

    def __init__(self, title: str):
        super().__init__(None)
        self.title = title
        
    def __str__(self):
        return self.title+"-"+super().__str__()

    @classmethod
    def from_dict(cls, d, parent):
        obj = MainWindow(d["title"])
        for child in d["children"]:
            cl = globals()[child["class"]]
            cl.from_dict(child,obj)
        return obj
        
class Layout(Widget):
    
    def __init__(self, parent, alignment: Alignment):
        super().__init__(parent)
        self.alignment = alignment

    def __str__(self):
        return str(self.alignment)+"-"+super().__str__()

    @classmethod
    def from_dict(cls, d, parent):
        obj = Layout(parent, Alignment.from_dict(d["alignment"],None))
        for child in d["children"]:
            cl = globals()[child["class"]]
            cl.from_dict(child,obj)
        return obj
        
class LineEdit(Widget):

    def __init__(self, parent, max_length: int=10):
        super().__init__(parent)  
        self.max_length = max_length

    def __str__(self):
        return str(self.max_length)+"-"+super().__str__()
        
    @classmethod
    def from_dict(cls, d, parent):
        obj = LineEdit(parent, d["max_length"])
        for child in d["children"]:
            cl = globals()[child["class"]]
            cl.from_dict(child,obj)
        return obj

class ComboBox(Widget):

    def __init__(self, parent, items):
        super().__init__(parent)
        self.items = items

    def __str__(self):
        return str(self.items)+"-"+super().__str__()

    @classmethod
    def from_dict(cls, d, parent):
        obj = LineEdit(parent, d["items"])
        for child in d["children"]:
            cl = globals()[child["class"]]
            cl.from_dict(child,obj)
        return obj

app = MainWindow("Application")
layout1 = Layout(app, Alignment.HORIZONTAL)
layout2 = Layout(app, Alignment.VERTICAL)

edit1 = LineEdit(layout1, 20)
edit2 = LineEdit(layout1, 30)

box1 = ComboBox(layout2, [1, 2, 3, 4])
box2 = ComboBox(layout2, ["a", "b", "c"])

print(app)

bts = app.to_binary()
print(f"Binary data length {len(bts)}")

new_app = MainWindow.from_binary(bts)
print(new_app)
