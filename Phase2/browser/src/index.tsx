import React from 'react';
import ReactDOM from 'react-dom';
import { BrowserRouter, Route, Routes } from "react-router-dom";
import Login from './Pages/Login';

ReactDOM.render(
  <>
    <BrowserRouter>
      <Routes>
        <Route path="/*" element={<Login />} />
      </Routes>
    </BrowserRouter>
  </>,
  document.getElementById('root')
);