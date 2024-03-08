import {WrapperConstructor} from 'mathjax-full/js/output/common/Wrapper';
import {JSONWrapper} from './Wrapper';
import {JSONmath} from './Wrappers/math';
import {JSONinferredMrow, JSONmrow} from './Wrappers/mrow';
import {JSONmi} from './Wrappers/mi';
import {JSONmo} from './Wrappers/mo';
import {JSONmn} from './Wrappers/mn';
import {JSONms} from './Wrappers/ms';
import {JSONmtext} from './Wrappers/mtext';
import {JSONmerror} from './Wrappers/merror';
import {JSONmspace} from './Wrappers/mspace';
import {JSONmpadded} from './Wrappers/mpadded';
import {JSONmphantom} from './Wrappers/mphantom';
import {JSONmfrac} from './Wrappers/mfrac';
import {JSONmsqrt} from './Wrappers/msqrt';
import {JSONmroot} from './Wrappers/mroot';
import {JSONmfenced} from './Wrappers/mfenced';
import {JSONmsub, JSONmsubsup, JSONmsup} from './Wrappers/msubsup';
import {JSONmover, JSONmunder, JSONmunderover} from './Wrappers/munderover';
import {JSONmmultiscripts} from './Wrappers/mmultiscripts';

import {JSONmtable} from './Wrappers/mtable';
import {JSONmlabeledtr, JSONmtr} from './Wrappers/mtr';
import {JSONmtd} from './Wrappers/mtd';

import {JSONmaction} from './Wrappers/maction';
import {JSONmenclose} from './Wrappers/menclose';
import {JSONannotation, JSONannotationXML, JSONsemantics, JSONxml} from './Wrappers/semantics';
import {JSONmglyph} from './Wrappers/mglyph';
import {JSONTeXAtom} from './Wrappers/TeXAtom';
import {JSONTextNode} from './Wrappers/TextNode';

export const JSONWrappers: { [kind: string]: WrapperConstructor } = {
    [JSONmath.kind]: JSONmath,
    [JSONmrow.kind]: JSONmrow,
    [JSONinferredMrow.kind]: JSONinferredMrow,
    [JSONmi.kind]: JSONmi,
    [JSONmo.kind]: JSONmo,
    [JSONmn.kind]: JSONmn,
    [JSONms.kind]: JSONms,
    [JSONmtext.kind]: JSONmtext,
    [JSONmerror.kind]: JSONmerror,
    [JSONmspace.kind]: JSONmspace,
    [JSONmpadded.kind]: JSONmpadded,
    [JSONmphantom.kind]: JSONmphantom,
    [JSONmfrac.kind]: JSONmfrac,
    [JSONmsqrt.kind]: JSONmsqrt,
    [JSONmroot.kind]: JSONmroot,
    [JSONmfenced.kind]: JSONmfenced,
    [JSONmsub.kind]: JSONmsub,
    [JSONmsup.kind]: JSONmsup,
    [JSONmsubsup.kind]: JSONmsubsup,
    [JSONmunder.kind]: JSONmunder,
    [JSONmover.kind]: JSONmover,
    [JSONmunderover.kind]: JSONmunderover,
    [JSONmmultiscripts.kind]: JSONmmultiscripts,
    [JSONmtable.kind]: JSONmtable,
    [JSONmtr.kind]: JSONmtr,
    [JSONmlabeledtr.kind]: JSONmlabeledtr,
    [JSONmtd.kind]: JSONmtd,
    [JSONmaction.kind]: JSONmaction,
    [JSONmenclose.kind]: JSONmenclose,
    [JSONsemantics.kind]: JSONsemantics,
    [JSONannotation.kind]: JSONannotation,
    [JSONannotationXML.kind]: JSONannotationXML,
    [JSONxml.kind]: JSONxml,
    [JSONmglyph.kind]: JSONmglyph,
    [JSONTeXAtom.kind]: JSONTeXAtom,
    [JSONTextNode.kind]: JSONTextNode,
    [JSONWrapper.kind]: JSONWrapper
};
