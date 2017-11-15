#!/usr/bin/env python
# -*- coding: utf-8 -*-

from __future__ import print_function, division

from goofit import *
import numpy as np

print_goofit_info()

xdata = np.random.exponential(size=100000)
xvar = Observable("xvar", 0, np.max(xdata) + 1)

ydata = np.random.exponential(size=100000)
yvar = Observable("yvar", 0, np.max(ydata) + 1)

varList = (xvar,yvar)

data = UnbinnedDataSet(xvar,yvar)

i = 0
while i < 100000:

    xvar.value = xvar.upperlimit - np.log(1+random()/2)
    yvar.value = yvar.upperlimit - np.log(1+random()/2)

    data.addEvent()
    i+=1

alpha_x = Variable("alpha_x", -2.4, 0.1, -10, 10)
alpha_y = Variable("alpha_y", -1.1, 0.1, -10, 10)

exp_x = ExpPdf("exp_x", xvar, alpha_x)
exp_y = ExpPdf("exp_x", yvar, alpha_y)
product = ProdPdf("product", (exp_x, exp_y))

product.setData(data)
fitter = FitManager(product)
fitter.fit()
