/**
 * Copyright (c) 2015, Jozef Stefan Institute, Quintelligence d.o.o. and contributors
 * All rights reserved.
 * 
 * This source code is licensed under the FreeBSD license found in the
 * LICENSE file in the root directory of this source tree.
 */
#ifndef QMINER_LA_NODEJS_H
#define QMINER_LA_NODEJS_H

#include "la_structures_nodejs.h"
#include "la_vector_nodejs.h"

///////////////////////////////
// NodeJs-Qminer-LinAlg
/**
* Linear algebra module.
* @module la
* @example
* // import module, create a random matrix and a vector, multiply. find svd of the matrix
*/
class TNodeJsLinAlg : public node::ObjectWrap {
	friend class TNodeJsUtil;
public:
	static void Init(v8::Handle<v8::Object> exports);

private:

	class TSVDTask : public TNodeTask {
	private:
		TNodeJsFltVV* JsFltVV;
		TNodeJsSpMat* JsSpVV;
		TNodeJsFltVV* U;
		TNodeJsFltVV* V;
		TNodeJsFltV* s;
		int k;
		int Iters;
		double Tol;

	public:
		TSVDTask(const v8::FunctionCallbackInfo<v8::Value>& Args);

		v8::Handle<v8::Function> GetCallback(const v8::FunctionCallbackInfo<v8::Value>& Args);
		void Run();
		v8::Local<v8::Value> WrapResult();
	};

public:
	/**
	* Computes the truncated SVD decomposition.
	* @param {module:la.Matrix | matrix:la.SparseMatrix} mat - The matrix.
	* @param {number} k - The number of singular vectors to be computed.
	* @param {Object} [json] - The JSON object containing the properties iter and tol:
	* @param {number} [json.iter = 100] - The number of iterations used for the algorithm.
	* @param {number} [json.tol = 1e-6] - The tolerance number.
	* @param {function} [callback] - The callback function, that takes the parameters err (error) and res (result). 
	* <i>Only for the asynchronous function.</i>
	* @returns {Object} The JSON object svdRes which contains the SVD decomposition U*S*V^T matrices:
	* <br>svdRes.U - The dense matrix of the decomposition. Type {@link module:la.Matrix}.
	* <br>svdRes.V - The dense matrix of the decomposition. Type {@link module:la.Matrix}.
	* <br>svdRes.s - The vector containing the singular values of the decomposition. Type {@link module:la.Vector}.
	* @example <caption>Asynchronous calculation</caption>
	* // import the modules
	* var la = require('qminer').la;
	* // create a random matrix
	* var A = new la.Matrix({ rows: 10, cols: 5, random: true });
	* // set the parameters for the calculation
	* var k = 2; // number of singular vectors 
	* var param = { iter: 1000, tol: 1e-4 };
	* // calculate the svd
	* la.svd(A, k, param, function (err, result) {
	*    if (err) { console.log(err); }
	*    // successful calculation
	*    var U = result.U;
	*    var V = result.V;
	*    var s = result.s;
	* });
	* @example <caption>Synchronous calculation</caption>
	* // import the modules
	* var la = require('qminer').la;
	* // create a random matrix
	* var A = new la.Matrix({ rows: 10, cols: 5, random: true });
	* // set the parameters for the calculation
	* var k = 2; // number of singular vectors
	* var param = { iter: 1000, tol: 1e-4 };
	* // calculate the svd
	* var result = la.svd(A, k, param);
	* // successful calculation
	* var U = result.U;
	* var V = result.V;
	* var s = result.s;
	*/
	//# exports.prototype.svd = function (mat, k, json) { return { U: Object.create(require('qminer').la.Matrix.prototype), V: Object.create(require('qminer').la.Matrix.prototype), s: Object.create(require('qminer').la.Vector.prototype) } }
	//JsDeclareFunction(svd);
	JsDeclareSyncAsync(svd, svdAsync, TSVDTask);

	/**
	* Computes the QR decomposition.
	* @param {module:la.Matrix} mat - The matrix.
	* @param {number} [tol = 1e-6] - The tolerance number.
	* @returns {Object} A JSON object qrRes which contains the decomposition matrices:
	* <br>qrRes.Q - The orthogonal dense matrix Q of the QR decomposition. Type {@link module:la.Matrix}.
	* <br>qrRes.R - The upper triangular dense matrix R of the QR decomposition. Type {@link module:la.Matrix}.
	*/
	//# exports.prototype.qr = function (mat, tol) { return { Q: Object.create(require('qminer').la.Matrix.prototype), R: Object.create(require('qminer').la.Matrix.prototype) } }
	JsDeclareFunction(qr);
};

#endif
