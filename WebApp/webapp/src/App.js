import logo from './logo.svg';
import './App.css';
import Main from './view/Main.jsx';
import WOL from './view/WOL.jsx'
import NavBar from './components/NavBar';
import {BrowserRouter, Routes, Route} from "react-router-dom";

function App() {
  return (
    <BrowserRouter>
      <NavBar/>
      <Routes>
        <Route path="/" element={<Main/>} />
        <Route path="/WOL" element={<WOL/>} />
      </Routes>
    </BrowserRouter>
  );
}

export default App;
