import React from 'react';
import ReactDOM from 'react-dom';
import { BrowserRouter, Route, Routes } from "react-router-dom";
import Login from './Pages/Login';
import '../node_modules/bootstrap/dist/css/bootstrap.min.css';
import HomePage from './Pages/HomePage';

ReactDOM.render(
  <>
    <BrowserRouter>
      <Routes>
        <Route path="/*" element={<Login />} />
        <Route path="/home" element={<HomePage />} />
      </Routes>
    </BrowserRouter>
  </>,
  document.getElementById('root')
);