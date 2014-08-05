package com.srch2.android.http.service;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.net.HttpURLConnection;
import java.net.URL;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.concurrent.RejectedExecutionException;

import android.util.Log;

abstract class HttpTask implements Runnable {

    protected StateResponseListener controlResponseObserver;
    protected SearchResultsListener searchResultsListener;

    static private boolean isExecuting = false;
    static private ExecutorService controlTaskExecutor;
    static private ExecutorService searchTaskExecutor;

    abstract protected void onTaskComplete(int returnedResponseCode,
                                           String returnedResponseLiteral);

    static synchronized void onStart() {
        isExecuting = true;
        controlTaskExecutor = Executors.newFixedThreadPool(1);
        searchTaskExecutor = Executors.newFixedThreadPool(1);
    }

    /**
     * Only for test purpose
     * @return
     */
    static boolean isExecuting(){
        return isExecuting;
    }

    static synchronized void onStop() {
        isExecuting = false;

        if (controlTaskExecutor != null) {
            controlTaskExecutor.shutdown();
        }

        if (searchTaskExecutor != null) {
            searchTaskExecutor.shutdown();
        }
    }

    static synchronized protected void executeTask(HttpTask taskToExecte) {
        if (!isExecuting || taskToExecte == null) {
            return;
        }
        Log.d("HttpTask", "1");
        boolean isSearchTask = taskToExecte.getClass() == SearchTask.class;

        if (isSearchTask && searchTaskExecutor != null) {
            try {
                searchTaskExecutor.execute(taskToExecte);
            } catch (RejectedExecutionException ignore) { /*
                                                         * Should not occur
														 * since isExecuting
														 * will control
														 * propagation.
														 */
            }
        } else if (!isSearchTask && controlTaskExecutor != null) {
            try {
                Log.d("HttpTask", "2");
                controlTaskExecutor.execute(taskToExecte);
            } catch (RejectedExecutionException ignore) { /*
														 * Should not occur
														 * since isExecuting
														 * will control
														 * propagation.
														 */
            }
        }
    }




    protected static abstract class SingleCoreHttpTask extends HttpTask {
        protected final String targetCoreName;

        protected SingleCoreHttpTask(String theTargetCoreName) {
            targetCoreName = theTargetCoreName;
        }
    }

    static abstract class ControlHttpTask extends SingleCoreHttpTask {
        protected final URL targetUrl;

        public ControlHttpTask(final URL theTargetUrl, final String theTargetCoreName, final StateResponseListener theControlResponseListener) {
            super(theTargetCoreName);
            targetUrl = theTargetUrl;
            controlResponseObserver = theControlResponseListener;
        }

        @Override
        protected void onTaskComplete(int returnedResponseCode,
                                      String returnedResponseLiteral) {
            if (SRCH2Engine.isChanged.get()) {
                SRCH2Engine.reQueryLastOne();
            }
        }


    }

    static abstract class SearchHttpTask extends SingleCoreHttpTask {
        protected final URL targetUrl;

        public SearchHttpTask(final URL theTargetUrl, final String theTargetCoreName, final SearchResultsListener theSearchResultsListener) {
            super(theTargetCoreName);
            targetUrl = theTargetUrl;
            searchResultsListener = theSearchResultsListener;
        }
    }

	protected String handleStreams(HttpURLConnection connection, String internalClassLogcatTag) throws IOException {
        String response = null;

        if (connection != null) {
            boolean hasValidInputStreamData = true;

            if (connection.getInputStream() != null) {
                response = readInputStream(connection.getInputStream());

                if (response == null || (response != null && response.trim().length() < 1)) {
                    Log.d(internalClassLogcatTag, "INPUT STREAM data is null");
                    hasValidInputStreamData = false;
                    response = null;
                } else {
                    Log.d(internalClassLogcatTag, "INPUT STREAM is _" + response + "_");
                }
            }else {
                Log.d(internalClassLogcatTag, "INPUT STREAM NULL");
            }

            if (!hasValidInputStreamData) {
                if (connection.getErrorStream() != null) {
                    response = readInputStream(connection.getErrorStream());
                    if (response == null || (response != null && response.trim().length() < 1)) {
                        Log.d(internalClassLogcatTag, "ERROR STREAM data is null");
                        response = null;
                    } else {
                        Log.d(internalClassLogcatTag, "ERROR STREAM is _" + response + "_");
                    }
                }else {
                    Log.d(internalClassLogcatTag, "ERROR STREAM NULL");
                }
            }
        }
        if (response == null) {
            response = RestfulResponse.IRRECOVERABLE_NETWORK_ERROR_MESSAGE;
        }
        return response;
    }

    protected void handleIOExceptionMessagePassing(IOException ioException, String response, String internalClassLogcatTag) {
        String errorResponse = null;
        if (ioException != null) {
            errorResponse = ioException.getMessage();
            if (errorResponse == null || (errorResponse != null && errorResponse.trim().length() < 1)) {
                errorResponse = null;
                Log.d(internalClassLogcatTag, "IOEXCEPTION message is NULL");
            } else {
                Log.d(internalClassLogcatTag, "IOEXCEPTION message is " + errorResponse);
            }
            ioException.printStackTrace();
        }

        if (errorResponse == null) {
            errorResponse = RestfulResponse.IRRECOVERABLE_NETWORK_ERROR_MESSAGE;
        }

        if (response == null) {
            response = errorResponse;
        } else {
            response += " " + errorResponse;
        }
    }

    /**
     * Reads from an input stream, returning the data read as a string encoded
     * as UTF-8. If the <code>InputStream source</code> is null, the returned
     * string will contain "NULL SOURCE INPUT STREAM".
     */
    final static String readInputStream(InputStream source) throws IOException {
        if (source == null) {
            return "NULL SOURCE INPUT STREAM";
        }
        StringBuilder sb = new StringBuilder();
        String line;
        BufferedReader reader = null;
        try {
            reader = new BufferedReader(new InputStreamReader(source, "UTF-8"));
            while ((line = reader.readLine()) != null) {
                sb.append(line);
            }
        } finally {
            if (reader != null) {
                reader.close();
            }
        }
        return sb.toString();
    }
	/*
	 * 
	 * SAVE for if we want to not use static, single instance of the thread
	 * pools in the future: ie the user wants multiple engine / client objects
	 * for some reason
	 * 
	 * // Wrapper class for controlling execution of tasks to be executed. Users
	 * of this class <b>must</b> call <code>onStart()</code> and
	 * <code>onStop()</code>. Users should // also obtain an instance of this
	 * class by calling <code>HttpTask.getHttpTaskExecutor()</code>. static
	 * class HttpTaskExecutor { private boolean isExecuting = false; private
	 * ExecutorService controlTaskExecutor; private ExecutorService
	 * searchTaskExecutor;
	 * 
	 * synchronized void onStart() { isExecuting = true; controlTaskExecutor =
	 * Executors.newFixedThreadPool(1); searchTaskExecutor =
	 * Executors.newFixedThreadPool(1); }
	 * 
	 * synchronized void onStop() { isExecuting = false;
	 * controlTaskExecutor.shutdown(); searchTaskExecutor.shutdown(); }
	 * 
	 * synchronized protected void executeTask(HttpTask taskToExecte) { if
	 * (!isExecuting || taskToExecte == null) { return; }
	 * 
	 * boolean isSearchTask = taskToExecte.getClass() == SearchTask.class;
	 * 
	 * if (isSearchTask && searchTaskExecutor != null) { try {
	 * searchTaskExecutor.execute(taskToExecte); } catch
	 * (RejectedExecutionException ignore) { } } else if (!isSearchTask &&
	 * controlTaskExecutor != null) { try {
	 * controlTaskExecutor.execute(taskToExecte); } catch
	 * (RejectedExecutionException ignore) { } } } }
	 * 
	 * static HttpTaskExecutor getHttpTaskExecutor() { return new
	 * HttpTaskExecutor(); }
	 */

}
