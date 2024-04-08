from enum import Enum
from typing import Callable, List

import numpy as np


class SourceType(Enum):
    HANDWRITING = 0
    SVG = 1


class TextType(Enum):
    LATEX = 0
    SMILES = 1


class TraverseOrder(Enum):
    DEFAULT = 0
    SORT_BY_MULTIPLY_DEMONSTRATION = 1


class PenContext:
    ...

    def register_draw_line(self, callback: Callable[[float, float, float, float], None]):
        """
        add a callback on draw_line event
        :param callback: invoke x0, y0, x1, y1 in from(x0,y0) and to(x1,y1)
        :return:
        """
        ...

    def register_draw_path(self, callback: Callable[[str, np.array], None]):
        """
        add a callback on draw_path event
        :param callback: invoke svg d element and 3x3 transform matrix
        :return:
        """
        ...

    def register_draw_text(self, callback: Callable[[str, np.array, np.array], None]):
        """
        add a callback on draw_text event
        :param callback: invoke text, bounding box and 3x3 transform matrix
        :return:
        """
        ...

    def register_on_label(self, callback: Callable[[str, str], None]):
        """
        (optional) invoke on meeting a new label
        :param callback: given unicode \\u{1D682} or '𝚂', we cluster it into ASCII code 'S',
            so we invoke origin label('𝚂'), fallback label('S').
            if we find no label in datasource, we may invoke a fallback label by unicode similarities.
            for molecules, invoke items at atom-bond level.
            for latex, invoke items at mathjax svg element level.
            if you need label for OCR training, always use origin label and do clustering yourself.
        :return:
        """
        ...

    def register_on_hierarchy(self, callback: Callable[[List[str], str], None]):
        """
        (optional) invoke on meeting a new hierarchy, for example, molecule bonds or mathml annotations
        :param callback:
            for molecule, invoke [from atom, to atom] and bond type when meeting bonds.
            for latex, invoke mathml stack and current mathjax svg element.
            label here is origin label, follow the same rule as register_on_label.
        :return:
        """
        ...


class PenOp:
    ...

    def add(self, offset: np.array):
        """
        translate a PenOp item
        :param offset:
        :return:
        """
        ...

    def dot(self, k: float):
        """
        scale a PenOp item
        :param k:
        :return:
        """
        ...

    def dot(self, kx: float, ky: float):
        """
        scale a PenOp item
        :param kx:
        :param ky:
        :return:
        """
        ...

    def bbox(self, parent_transform: np.array) -> np.array:
        """
        get bounding box of current PenOp item
        :param parent_transform:
        :return:
        """
        ...

    def bbox(self) -> np.array:
        """
        get bounding box of current PenOp item
        :return:
        """
        ...

    def fit_into_keep_ratio(self, w: float, h: float):
        """
        fit current PenOp item into a bounding box of (w, h), keep origin ratio
        :param w:
        :param h:
        :return:
        """
        ...

    def fit_into(self, w: float, h: float):
        """
        fit current PenOp item into a bounding box of (w, h), ignore origin ratio
        :param w:
        :param h:
        :return:
        """
        ...

    def move_center_to(self, point: np.array):
        """
        move current PenOp item's center to a point
        :param point:
        :return:
        """
        ...

    def rotate_deg(self, degree: float):
        """
        rotate current PenOp item in degrees
        :param degree:
        :return:
        """
        ...

    def rotate(self, radians: float):
        """
        rotate current PenOp item in radians
        :param radians:
        :return:
        """
        ...

    def rotate(self, radians: float, center: np.array):
        ...

    def set_context(self, context: PenContext):
        """
        register drawing context for user callback
        :param context:
        :return:
        """
        ...

    def on_paint(self):
        """
        traverse current PenOp item and perform drawing
        :return:
        """
        ...

    def on_paint(self, parent_transform: np.array):
        """
        traverse current PenOp item and perform drawing, apply a 3x3 transform matrix before invoking context callback
        :param parent_transform:
        :return:
        """
        ...


def global_init_couch(root: str, components: [str]):
    """
    init handwriting meta fonts from Couch dataset, use std::call_once initialization and c++ static storage
    :param root: path to SCUT_IRAC/Couch
    :param components: Couch subset, for example, Couch_Digit_195
    :return:
    """
    ...


def global_init_makemeahanzi(path: str):
    """
    init chinese svg fonts from makemeahanzi, use std::call_once initialization and c++ static storage
    :param path: path to makemeahanzi/graphics.txt
    :return:
    """
    ...


def generate(
        *,
        text: str,
        source_type: SourceType,
        text_type: TextType,
        traverse_order=TraverseOrder.DEFAULT,
) -> PenOp:
    """
    a wrapper for SynthesisTexGenerator and StructuralFormulaItem in c++
    :param text: content of input
    :param source_type: svg or handwriting
    :param text_type: latex or smiles
    :param traverse_order: may be helpful when demonstrating drafts of multiplication
    :return: :class:`PenOp`
    """
    ...


def generate_batch(
        *,
        texts: List[str],
        source_type: SourceType,
        text_type: TextType,
        traverse_order=TraverseOrder.DEFAULT,
) -> List[PenOp]:
    """
    a wrapper for SynthesisTexGenerator and StructuralFormulaItem in c++, enable multithreading with taskflow
    :param texts: list of input content
    :param source_type: svg or handwriting
    :param text_type: latex or smiles
    :param traverse_order: may be helpful when demonstrating drafts of multiplication
    :return: :class:`PenOp`
    """
    ...

def generate_batch_image(
        *,
        texts: List[str],
        source_type: SourceType,
        text_type: TextType,
        traverse_order=TraverseOrder.DEFAULT,
) -> List[bytes]:
    """
    a wrapper for SynthesisTexGenerator and StructuralFormulaItem in c++, enable multithreading with taskflow
    this API is used for massive image generation
    :param texts: list of input content
    :param source_type: svg or handwriting
    :param text_type: latex or smiles
    :param traverse_order: may be helpful when demonstrating drafts of multiplication
    :return: list of png buffer
    """
    ...

def generate_multiply_draft_latex(a: str, b: str) -> str:
    """
     generate latex like:
           1.2
      x    3.4
      ---------
           4 8
         3 6
      ---------
         4.0 8
    :param a: string of a number, can be floating point like "1.2"
    :param b: string of a number, can be floating point like "3.4"
    :return: a latex string representing draft of a times b
    """
    ...
